#!/usr/bin/env python
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
"""Simple example script that uses pw_rpc."""

import argparse
import logging
import os
from pathlib import Path
import sys
from typing import Callable, List, Tuple

import serial  # type: ignore

from pw_hdlc_lite.rpc import HdlcRpcClient
from pw_tokenizer.detokenize import AutoUpdatingDetokenizer, detokenize_base64

# Point the script to the .proto file with our RPC services.
PROTO = Path(os.environ['PW_ROOT']) / 'pw_rpc' / 'pw_rpc_protos' / 'echo.proto'

_LOG = logging.getLogger('factory_test_example')


class TestFailure(Exception):
    """Indicates that a test failed."""


FailedTests = List[Tuple[Callable, TestFailure]]

# Set up a detokenizer so logs can be printed in plain text.
_detokenizer = AutoUpdatingDetokenizer(
    Path(__file__).parent / 'tokenizer_database.csv')


def detokenize_and_print(data: bytes):
    print(detokenize_base64(_detokenizer, data).decode(errors='replace'))


class TestContext:
    """Used to track the state of ongoing tests."""
    def __init__(self, client: HdlcRpcClient):
        self.client = client
        self.failed: FailedTests = []
        self._current_test: str = ''

    @property
    def rpcs(self):
        """Returns the RPC accessor object for the device."""
        return self.client.rpcs()

    def run(self, test: Callable) -> None:
        """Runs a function as a factory test."""
        try:
            _LOG.info('%s: Running test', test.__name__)
            test(self)
            _LOG.info('%s: PASSED ', test.__name__)
        except TestFailure as failure:
            self.failed.append((test, failure))
            _LOG.info('%s: FAILED - %s', test.__name__, failure)


def expect_ok(result: tuple):
    """Helper to check the result of a unary RPC."""

    status, value = result
    if status.ok():
        return value

    raise TestFailure(str(status))


def test_echo(ctx: TestContext) -> None:
    """Tests that the device responds to RPCs as expected."""
    for test_str in ['', 'abc', 'This is a test']:
        reply = expect_ok(ctx.rpcs.pw.rpc.EchoService.Echo(msg=test_str))

        if reply.msg != test_str:
            raise TestFailure(f'Expected {test_str!r}, got {reply.msg!r}')


def run_tests(device: str, baud: int) -> FailedTests:
    """Runs the factory tests."""

    # Set up a pw_rpc client that uses HDLC.
    hdlc_client = HdlcRpcClient(serial.Serial(device, baud), [PROTO],
                                output=detokenize_and_print)

    # Create a TestContext and run the tests with it.
    tester = TestContext(hdlc_client)
    tester.run(test_echo)

    return tester.failed


def main() -> int:
    """Parses the command line args and runs the tests."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--device',
                        '-d',
                        default='/dev/ttyACM0',
                        help='serial device to use')
    parser.add_argument('--baud',
                        '-b',
                        type=int,
                        default=115200,
                        help='baud rate for the serial device')

    return 1 if run_tests(**vars(parser.parse_args())) else 0


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())
