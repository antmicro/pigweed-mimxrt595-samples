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
