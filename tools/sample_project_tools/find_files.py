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
"""File finding utility."""

import argparse
import logging
import os
import sys
import shlex
import subprocess
from pathlib import Path

import pw_cli.log

_LOG = logging.getLogger(__name__)


def error_unknown_arg(unknown_arg):
    _LOG.error('Unrecognized argument: %s', unknown_arg)
    _LOG.info('')
    _LOG.info('Did you mean to pass this argument to the exec command?')
    _LOG.info('Insert a -- in front of it to forward it through:')
    _LOG.info('')

    index = sys.argv.index(unknown_arg)
    args_rest = sys.argv[index:]
    if "--" in args_rest:
        args_rest.remove("--")
    fixed_cmd = [*sys.argv[:index], '--', *args_rest]

    _LOG.info('  %s', ' '.join(shlex.quote(arg) for arg in fixed_cmd))
    _LOG.info('')


def build_argument_parser():
    """Setup find-files argparse."""
    def log_level(arg: str) -> int:
        try:
            return getattr(logging, arg.upper())
        except AttributeError:
            raise argparse.ArgumentTypeError(
                f'{arg.upper()} is not a valid log level')

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-l',
                        '--loglevel',
                        type=log_level,
                        default=logging.INFO,
                        help='Set the log level '
                        '(debug, info, warning, error, critical)')
    parser.add_argument("-s",
                        "--starting-dir",
                        default=os.getcwd(),
                        help="The starting directory to run a find. "
                        "Default: {}".format(os.getcwd()))
    parser.add_argument("--type",
                        dest="file_type",
                        choices=["d", "f"],
                        help="Limit results to directories 'd' or files 'f'.")
    parser.add_argument("-p",
                        "--pattern",
                        metavar="PATTERN",
                        dest="patterns",
                        action="append",
                        required=True,
                        help="Glob patterns to search with. Wildcard is '*'. "
                        "The '**' pattern means 'this directory and all "
                        "subdirectories, recursively'. Multiple patterns can "
                        "be used and are combined with a logical or.")
    parser.add_argument('exec_args',
                        metavar="-- EXEC_ARGS",
                        nargs=argparse.REMAINDER,
                        help="Run a command on each file. The command should "
                        "be specified at the end after a '--'. Any args "
                        "matching '%%f' will be replaced with the file name.")
    return parser


def main() -> int:
    """Simple find-file utility similar to the Unix find command."""

    parser = build_argument_parser()
    args, unused_extra_args = parser.parse_known_args()
    pw_cli.log.install(args.loglevel)
    if unused_extra_args:
        error_unknown_arg(unused_extra_args[0])
        return 1

    starting_dir = os.path.realpath(
        os.path.expanduser(os.path.expandvars(args.starting_dir)))
    if not os.path.exists(starting_dir):
        _LOG.error("Starting directory '%s' not found.", args.starting_dir)
        return 1

    results = []
    for pattern in args.patterns:
        for path in Path(starting_dir).glob(pattern):
            if args.file_type:
                if args.file_type == "d" and path.is_dir():
                    results.append(path)
                elif args.file_type == "f" and path.is_file():
                    results.append(path)
            else:
                results.append(path)

    exec_args = []
    if args.exec_args:
        if args.exec_args[0] != '--':
            error_unknown_arg(args.exec_args[0])
            return 1
        exec_args = args.exec_args[1:]

    for file_name in [
            str(p.relative_to(os.getcwd())) for p in sorted(results)
    ]:
        print(file_name)
        if exec_args:
            command = [file_name if arg == "%f" else arg for arg in exec_args]
            _LOG.debug("Running: %s", " ".join(command))
            subprocess.run(command)
    return 0


if __name__ == '__main__':
    sys.exit(main())
