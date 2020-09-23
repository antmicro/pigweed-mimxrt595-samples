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
