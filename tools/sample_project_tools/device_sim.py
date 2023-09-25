# Copyright 2023 The Pigweed Authors
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
"""Launches a simulated device subprocess under pw_console."""

import argparse
from pathlib import Path
import subprocess
import sys

from pw_system.console import get_parser
from sample_project_tools import console


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        'sim_binary',
        type=Path,
        help='Path to a simulated device binary to run on the host',
    )
    return parser.parse_known_args()[0]


def launch_sim(sim_binary: Path) -> int:
    """Launches a host-device-sim binary, and attaches a console to it."""

    sim_process = subprocess.Popen(
        sim_binary,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.STDOUT,
    )

    try:
        retval = console.main(get_parser().parse_args(['-s', 'default']))
    except:
        sim_process.terminate()
        raise

    sim_process.terminate()

    return retval


def main() -> int:
    return launch_sim(**vars(_parse_args()))


if __name__ == '__main__':
    sys.exit(main())
