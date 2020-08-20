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

## Sample Application
The sample application in `source/main.cc` uses the sample module
`simple_counter`. Look at `source/BUILD.gn` and `source/simple_counter/BUILD.gn`
to see how these are built respectively. The key part is in the root `BUILD.gn`,
which creates the host target using the host toolchain. A toolchain is required
for each target.

## Sample Test
The `simple_counter` module has tests defined in `source/simple_counter_tests.cc`.
Look at `source/simple_counter/BUILD.gn` for an example of how a test is
defined. The root `BUILD.gn` groups all the host tests together.
