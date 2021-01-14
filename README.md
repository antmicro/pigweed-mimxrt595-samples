# Pigweed Sample Project

[TOC]

This repository outlines the recommended way of using Pigweed in a new or
existing project. Feel free to fork this repository, or read it as a reference.

For more information see the [Pigweed Getting started
guide](https://pigweed.googlesource.com/pigweed/pigweed/+/refs/heads/master/docs/getting_started.md)

Check back for more complex examples and features coming soon!

## Repository setup

Clone this repo with `--recursive` to get all required submodules.

```sh
git clone --recursive https://pigweed.googlesource.com/pigweed/sample_project
```

This will pull the [Pigweed source
repository](https://pigweed.googlesource.com/pigweed/pigweed) into
`third_party/pigweed`. If you already cloned but forgot to `--recursive` run
`git submodule update --init` to pull all submodules.

## Environment Setup

After cloning the build tools will need to be installed with the `bootstrap`
scripts. This is only required after the initial clone or updating Pigweed.

**Windows**

```sh
bootstrap.bat
```

**Linux & Mac**

```sh
source bootstrap.sh
```

`bootstrap.bat` and `bootstrap.sh` call the respective Pigweed bootstrap
scripts. Feel free to modify them to set up your development environment for the
first time.

After the initial bootstrap, use the `activate` scripts to setup your shell for development.

**Windows**

```sh
activate.bat
```

**Linux & Mac**

```sh
source activate.sh
```

## Banner

Make the project yours with your own banner! Edit `banner.txt` as desired `:-D`.

## Building

Generate the build files with `gn gen out` once, unless the build configuration
has changed. Then, use ninja to build everything with `ninja -C out`.

The sample project examples have dependencies, each with some required build
arguments. See the [Arduino](#arduino-example), and [RPC](#rpc-example) sections
for more setup information. The build arguments can be set with `gn args out`.

## Pigweed's File System Watcher

Speed up iteration running `pw watch` on a new terminal window. This utility
builds targets and runs tests when their files are modified. See the `pw_watch`
documentation for more information.

## Presubmits

Sample presubmit checks are included in `tools/presubmit_checks.py`. To install
them run `python tools/presubmit_checks.py --install`. See the `pw_presubmit`
module documentation for more information.

## Sample Application

The sample application in `source/main.cc` uses the sample module
`simple_counter`. Look at `source/BUILD.gn` and `source/simple_counter/BUILD.gn`
to see how these are built respectively. The key part is in the root `BUILD.gn`,
which creates the host target using the host toolchain. A toolchain is required
for each target.

[Build](#building) the project and run the application.

```sh
./out/host_clang_debug/obj/source/bin/hello_world
```

## Sample Test

The `simple_counter` module has tests defined in
`source/simple_counter_tests.cc`. Look at `source/simple_counter/BUILD.gn` for
an example of how a test is defined. The root `BUILD.gn` groups all the host
tests together.

[Build](#building) the project and run the tests.

```sh
./out/host_clang_debug_tests/obj/source/simple_counter/test/simple_counter_test
```

## Arduino Example

### 1. Install an Arduino Core

To build for Arduino boards you must install a core. At this time only the
[Teensyduino core](https://www.pjrc.com/teensy/td_download.html) is supported.

Check the Pigweed `pw_arduino_build` module documentation for detailed
installation instructions. Cores should be installed into
`third_party/piwgweed/third_party/arduino`.

Run this to install the Teensy core:

```sh
arduino_builder install-core --prefix ./third_party/pigweed/third_party/arduino/cores/ --core-name teensy
```

### 2. Build

To build for a Teensy 4.0 board run the following.

**Linux & Mac**

```sh
gn gen out --args='
  pw_arduino_build_CORE_PATH="//third_party/pigweed/third_party/arduino"
  pw_arduino_build_CORE_NAME="teensy"
  pw_arduino_build_BOARD="teensy40"'
ninja -C out
```

**Windows**

Run `gn gen out` which will open a text editor. Paste in the following, save and
close the editor.

```sh
pw_arduino_build_CORE_PATH="//third_party/pigweed/third_party/arduino"
pw_arduino_build_CORE_NAME="teensy"
pw_arduino_build_BOARD="teensy40"
```

Where `pw_arduino_build_BOARD=` is one of:

- `"teensy31"` - Teensy 3.2 / 3.1
- `"teensy35"` - Teensy 3.5
- `"teensy36"` - Teensy 3.6
- `"teensy40"` - Teensy 4.0
- `"teensy41"` - Teensy 4.1

### 3. Run a Test

#### Manual Test

Tests can be manually flashed an run with the `arduino_unit_test_runner`
and the `.elf` file.

```sh
arduino_unit_test_runner --verbose \
    --config-file out/arduino_debug/gen/arduino_builder_config.json \
    --upload-tool teensyloader \
    out/arduino_debug_tests/obj/source/simple_counter/test/simple_counter_test.elf
```

**Single line:**
```sh
arduino_unit_test_runner --verbose --config-file out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader out/arduino_debug_tests/obj/source/simple_counter/test/simple_counter_test.elf
```

#### Flash Only

A given binary can be flashed without checking for test pass or fail messages
with the `--flash-only` option:

```sh
arduino_unit_test_runner --verbose \
  --config out/arduino_debug/gen/arduino_builder_config.json \
  --upload-tool teensyloader \
  --flash-only \
  out/arduino_debug_tests/obj/source/simple_counter/test/simple_counter_test.elf
```

**Single line:**
```sh
arduino_unit_test_runner --verbose --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --flash-only out/arduino_debug_tests/obj/source/simple_counter/test/simple_counter_test.elf
```

#### Unit Test Server

If you would like to use the unit test server to automatically run your tests
you must set the `pw_arduino_use_test_server=true` build arg and startup the
test server. Then in a second window start the `pw watch` command.

1.  Start the test server in it's own terminal window.

    ```sh
    gn gen out --args='
      pw_arduino_build_CORE_PATH="//third_party/pigweed/third_party/arduino"
      pw_arduino_build_CORE_NAME="teensy"
      pw_arduino_build_BOARD="teensy40"
      pw_arduino_use_test_server=true'
    arduino_test_server --verbose --config-file ./out/arduino_debug/gen/arduino_builder_config.json
    ```

1.  Start `pw watch out` in a separate terminal.

    ```sh
    pw watch out
    ```

For additional details check the Pigweed `arduino_builder` documentation in:

- [pigweed.dev/pw_arduino_build/](https://pigweed.dev/pw_arduino_build/)
- [pigweed.dev/pw_sys_io_arduino/](https://pigweed.dev/pw_sys_io_arduino/)
- [pigweed.dev/targets/arduino/target_docs](https://pigweed.dev/targets/arduino/target_docs.html)

## Tokenized Logging

Log entries in the sample app are tokenized to save binary space. The included
tokens database, `source/tokenizer_database.csv`, is updated on each build. See
the Pigweed `pw_tokenizer` for more information.

Optionally, the database can be created manually using the binary or the .elf
file.

```sh
python -m pw_tokenizer.database create \
    --database source/tokenizer_database.csv \
    out/host_clang_debug/obj/source/bin/hello_world
```

**Single line:**
```sh
python -m pw_tokenizer.database create --database source/tokenizer_database.csv out/host_clang_debug/obj/source/bin/hello_world
```

Running the app shows log entries similiar to `$kgjLdg==`. These can be saved to
a file and then detokenized.

```sh
./out/host_clang_debug/obj/source/bin/hello_world > log.txt
python -m pw_tokenizer.detokenize base64 source/tokenizer_database.csv -i log.txt
```

Or can be detokenized in realtime.

```sh
./out/host_clang_debug/obj/source/bin/hello_world | python \
    -m pw_tokenizer.detokenize base64 source/tokenizer_database.csv
```

## RPC Example

The sample project uses nanopb for its `pw_rpc` dependency. The nanopb repo is
conveniently included as a git submodule. This installation can be overridden
following the instructions in `third_party/pigweed/third_party/nanopb/BUILD.gn`.
Then set `dir_pw_third_party_nanopb` to the new installation location when
building. For example:

```sh
gn gen out --args='dir_pw_third_party_nanopb="third_party/nanopb"'
```

The sample application registers the `EchoService`, which echoes any RPC message
data sent to it. To test it out build for and flash the desired board, then run:

```sh
python third_party/pigweed/pw_hdlc/rpc_example/example_script.py --device /dev/ttyACM0 --baud 115200
```

At the time of writing, the `example_script.py` does not parse log statements if
they are not framed in the HDLC protocol used by RPC. There is ongoing work on
an RPC log service that will handle sending logs in HDLC frames to an RPC
channel. In addition, the sample application uses tokenized logging, which means
that logs need to be detokenized after the RPC HDLC frames are decoded on the
host side. To do that flash the hello_word application and use two terminals to
parse RPCs and detokenize logs respectively.

Terminal 1: receive RPCs.

```sh
source activate.sh
python -m pw_hdlc.rpc_console \
       -o logfile.txt \
       -d /dev/ttyACM0 \
       ./third_party/pigweed/pw_rpc/pw_rpc_protos/echo.proto
```

Test Echo RPC:

```sh
rpcs.pw.rpc.EchoService.Echo(msg="hola")
```

Terminal 2: decode the logfile.

```sh
source activate.sh
tail -f logfile.txt | python -m pw_tokenizer.detokenize \
    base64 source/tokenizer_database.csv
```
