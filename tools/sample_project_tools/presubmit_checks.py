#!/usr/bin/env python3
# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Example presubmit check script."""

import argparse
import logging
import os
from pathlib import Path
import re
import sys

try:
    import pw_cli.log
except ImportError:
    print(
        'ERROR: Activate the environment before running presubmits!',
        file=sys.stderr,
    )
    sys.exit(2)

import pw_presubmit
from pw_presubmit import (
    build,
    cli,
    cpp_checks,
    format_code,
    git_repo,
    inclusive_language,
    install_hook,
    keep_sorted,
    python_checks,
)
from pw_presubmit.presubmit_context import PresubmitContext, PresubmitFailure

_LOG = logging.getLogger(__name__)

# Set up variables for key project paths.
try:
    PROJECT_ROOT = Path(os.environ['SAMPLE_PROJECT_ROOT'])
except KeyError:
    print(
        'ERROR: The presubmit checks must be run in the sample project\'s root'
        ' directory',
        file=sys.stderr,
    )
    sys.exit(2)

PIGWEED_ROOT = PROJECT_ROOT / 'third_party' / 'pigweed'

# Rerun the build if files with these extensions change.
_BUILD_EXTENSIONS = frozenset(
    ['.rst', '.gn', '.gni', *format_code.C_FORMAT.extensions]
)


#
# Presubmit checks
#
def default_build(ctx: PresubmitContext):
    """Creates a default build."""
    build.gn_gen(ctx)
    build.ninja(ctx)


def check_for_git_changes(ctx: PresubmitContext):
    """Checks that repositories have all changes committed."""
    checked_repos = (PIGWEED_ROOT, *ctx.repos)
    changes = [r for r in checked_repos if git_repo.has_uncommitted_changes(r)]
    for repo in changes:
        _LOG.error('There are uncommitted changes in the %s repo!', repo.name)
    if changes:
        _LOG.warning(
            'Commit or stash pending changes before running the presubmit.'
        )
        raise PresubmitFailure


# Avoid running some checks on certain paths.
PATH_EXCLUSIONS = (
    re.compile(r'^external/'),
    re.compile(r'^third_party/'),
    re.compile(r'^vendor/'),
)

#
# Presubmit check programs
#
OTHER_CHECKS = (build.gn_gen_check,)

QUICK = (
    default_build,
    format_code.presubmit_checks(
        code_formats=format_code.CODE_FORMATS_WITH_BLACK
    ),
)

LINTFORMAT = (
    # keep-sorted: start
    cpp_checks.pragma_once,
    format_code.presubmit_checks(),
    inclusive_language.presubmit_check,
    keep_sorted.presubmit_check,
    python_checks.gn_python_lint,
    # keep-sorted: end
)

FULL = (
    QUICK,  # Add all checks from the 'quick' program
    LINTFORMAT,
    # Use the upstream Python checks, with custom path filters applied.
    python_checks.gn_python_check,
)

PROGRAMS = pw_presubmit.Programs(
    # keep-sorted: start
    full=FULL,
    lintformat=LINTFORMAT,
    other_checks=OTHER_CHECKS,
    quick=QUICK,
    # keep-sorted: end
)


def run(install: bool, exclude: list, **presubmit_args) -> int:
    """Process the --install argument then invoke pw_presubmit."""

    # Install the presubmit Git pre-push hook, if requested.
    if install:
        install_hook.install_git_hook(
            'pre-push',
            [
                'python',
                '-m',
                'sample_project_tools.presubmit_checks',
                '--base',
                'origin/main..HEAD',
                '--program',
                'quick',
            ],
        )
        return 0

    exclude.extend(PATH_EXCLUSIONS)
    repos = git_repo.discover_submodules(superproject_dir=PROJECT_ROOT)
    return cli.run(
        root=PROJECT_ROOT, repositories=repos, exclude=exclude, **presubmit_args
    )


def main() -> int:
    """Run the presubmit checks for this repository."""
    parser = argparse.ArgumentParser(description=__doc__)
    cli.add_arguments(parser, PROGRAMS, 'quick')

    # Define an option for installing a Git pre-push hook for this script.
    parser.add_argument(
        '--install',
        action='store_true',
        help='Install the presubmit as a Git pre-push hook and exit.',
    )

    return run(**vars(parser.parse_args()))


if __name__ == '__main__':
    pw_cli.log.install(logging.INFO)
    sys.exit(main())
