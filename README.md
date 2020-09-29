# Pigweed Sample Project

This repository outlines the recommended way of using Pigweed in a new or
existing project. Feel free to fork this repository, or read it as a reference.

For more information see the [Pigweed Getting started guide](https://pigweed.googlesource.com/pigweed/pigweed/+/refs/heads/master/docs/getting_started.md)

Check back for more complex examples and features coming soon!

## Repository setup
The preferred method to add the Pigweed source is to use git submodules and
place the [Pigweed source repository](https://pigweed.googlesource.com/pigweed/pigweed)
in `third_party/pigweed`.  If you forked this repository, don't forget to run
`git submodule init` and `git submodule update`.

## Environment Setup
The scripts `bootstrap.bat` for Windows, and `bootstrap.sh` for Unix systems
call the respective Pigweed bootstrap scripts. Feel free to modify them to set
up your development environment for the first time.

After the initial setup, use the `activate.bat` in Windows or `activate.sh` in
Unix to enter the environment in a shell.

## Banner
Make the project yours with your own banner. Create your own banner and place it
in `banner.txt`.

## Building
Generate the build files with `gn gen out` once, unless the build configuration
has changed. Then, use ninja to build everything with `ninja -C out`.

The sample project examples have dependencies, each with some required build
arguments. See the [Arduino](#arduino-example), and [RPC](#rpc-example) sections
for more setup information. The build arguments can be set with `gn args out`.

## Sample Application
The sample application in `source/main.cc` uses the sample module
`simple_counter`. Look at `source/BUILD.gn` and `source/simple_counter/BUILD.gn`
to see how these are built respectively. The key part is in the root `BUILD.gn`,
which creates the host target using the host toolchain. A toolchain is required
for each target.

[Build](#building) the project and run the application.
`./out/host_clang_debug/obj/source/bin/hello_world`

## Sample Test
The `simple_counter` module has tests defined in `source/simple_counter_tests.cc`.
Look at `source/simple_counter/BUILD.gn` for an example of how a test is
defined. The root `BUILD.gn` groups all the host tests together.

[Build](#building) the project and run the tests.
`./out/host_clang_debug_tests/obj/source/simple_counter/test/simple_counter_test`

## Tokenized Logging
Log entries in the sample app are tokenized to save binary space. The included
tokens database, `source/tokenizer_database.csv`, is updated on each build. See
the Pigweed `pw_tokenizer` for more information.

Optionally, the database can be created manually using the binary or the .elf
file.
`python -m pw_tokenizer.database create --database source/tokenizer_database.csv out/host_clang_debug/obj/source/bin/hello_world`

Running the app shows log entries similiar to `$kgjLdg==`. These can be saved to
a file and then detokenized.

```./out/host_clang_debug/obj/source/bin/hello_world > log.txt
python -m pw_tokenizer.detokenize base64 source/tokenizer_database.csv -i log.txt
```

Or can be detokenized in realtime.
`./out/host_clang_debug/obj/source/bin/hello_world | python -m pw_tokenizer.detokenize base64 source/tokenizer_database.csv`

## Arduino Example
Follow the Pigweed `pw_arduino` module documentation to install the Arduino core
software under `third_party/piwgweed/third_party/arduino`. There is an example
application under `source/arduino_example` that uses the common utilities from
`Arduino.h`.The `source/arduino_example/BUILD.gn` demonstrates how to include
the arduino core dependencies.
The sample application in `source/main.cc` can also be compiled for a supported
Arduino board. The `BUILD.gn` shows how to create a target using the arduino
toolchain.
The `source/target/arduino` contains a sample toolchain that inherits from the
an arduino toolchain in upstream pigweed. It can override backends as needed.

To build for a Teensy 3.1 board simply run the following.
```
gn gen out --args="arduino_board=\"teensy31\"\
    dir_pw_third_party_arduino=\"//third_party/pigweed/third_party/arduino\""
ninja -C out
```

## RPC Example
The sample project uses nanopb for its `pw_rpc` dependency. The nanopb repo is
conviniently included as a git submodule. This installation can be overridden
following the instructions in `third_party/pigweed/third_party/nanopb/BUILD.gn`.
Then set `dir_pw_third_party_nanopb` to the new installation location when
building. For example:
`gn gen out --args="dir_pw_third_party_nanopb=\"third_party/nanopb\""`

The sample application registers the `EchoService`, which echoes any RPC message
data sent to it. To test it out build for and flash the desired board, then run:
`python third_party/pigweed/pw_hdlc_lite/rpc_example/example_script.py --device /dev/ttyACM0 --baud 115200`.

At the time of writing, the `example_script.py` does not parse log statements if
they are not framed in the HDLC protocol used by RPC. There is ongoing work on
an RPC log service that will handle sending logs in HDLC frames to an RPC
channel. In addition, the sample application uses tokenized logging, which means
that logs need to be detokenized after the RPC HDLC frames are decoded on the
host side. To do that flash the hello_word application and use two terminals to
parse RPCs and detokenize logs respectively.

Terminal 1: receive RPCs.
```
source activate.sh
python -m pw_hdlc_lite.rpc_console -o logfile.txt -d /dev/ttyACM0 ./third_party/pigweed/pw_rpc/pw_rpc_protos/echo.proto
```
Test Echo RPC:
`rpcs.pw.rpc.EchoService.Echo(msg="hola")`


Terminal 2: decode the logfile.
```
source activate.sh
tail -f logfile.txt | python -m pw_tokenizer.detokenize base64 source/tokenizer_database.csv
```
