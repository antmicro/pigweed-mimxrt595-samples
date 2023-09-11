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
"""A handy wrapper that simplifies flashing a device."""

import argparse
import logging
import os
from pathlib import Path
import subprocess
import sys


_LOG = logging.getLogger('flash_device')


def _log_subprocess_output(level, output) -> None:
    """Logs subprocess output line-by-line."""

    lines = output.decode('utf-8', errors='replace').splitlines()
    for line in lines:
        _LOG.log(level, line)


def _get_environment_variable(variable_name: str) -> str:
    val = os.environ.get(variable_name)
    if not val:
        raise ValueError(
            f'The environment variable `{variable_name}` isn\'t '
            'set, please activate the Pigweed environment first by sourcing'
            'bootstrap.sh:\n'
            '  $ . bootstrap.sh'
        )
    return val


def flash_stm32_discovery(firmware_image: Path) -> bool:
    """Flashes a firmware image to a STM32F429I-DISC1 board."""
    # TODO: This should use a real, shared flashing script.
    cipd_install_dir = _get_environment_variable('PW_PIGWEED_CIPD_INSTALL_DIR')
    pigweed_root = _get_environment_variable('PW_ROOT')

    cmd = (
        'openocd',
        '-s',
        f'{cipd_install_dir}/share/openocd/scripts',
        '-f',
        f'{pigweed_root}/targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg',  # pylint: disable=line-too-long
        '-c',
        f'program {firmware_image} reset exit',
    )

    process = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )

    if process.returncode:
        _log_subprocess_output(logging.ERROR, process.stdout)
        raise ValueError('Failed to flash target device')

    return process.returncode == 0


_SUPPORTED_DEVICES = {
    'STM32-Discovery': flash_stm32_discovery,
}


def _parse_args() -> argparse.Namespace:
    """Parses commandline arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'firmware_image',
        type=Path,
        help='Path to the firmware image to flash to an attached device.',
    )
    parser.add_argument(
        '--device',
        '-d',
        dest='dev_name',
        type=str,
        required=True,
        choices=_SUPPORTED_DEVICES.keys(),
        help='Device type, selects the flashing command to use',
    )
    return parser.parse_args()


def flash_device(dev_name: str, firmware_image: Path) -> bool:
    """Flashes the specified firmware image to a device."""
    if dev_name not in _SUPPORTED_DEVICES:
        raise ValueError(
            f'Unknown device `{dev_name}`, '
            f'please choose one of {_SUPPORTED_DEVICES.keys()}'
        )
    _LOG.info('Flashing %s via the %s tool...', firmware_image, dev_name)
    if _SUPPORTED_DEVICES[dev_name](firmware_image):
        _LOG.info('Done!')
        return True

    return False


def main() -> int:
    """Parses commandline arguments and attempts to flash a device."""
    return 0 if flash_device(**vars(_parse_args())) else 1


if __name__ == '__main__':
    sys.exit(main())
