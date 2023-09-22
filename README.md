# Pigweed Sample Project

[TOC]

This repository outlines the recommended way of using Pigweed in a new or
existing project. Feel free to fork this repository, or read it as a reference.

For more information see the [Pigweed Getting started
guide](https://pigweed.dev/docs/getting_started.html).

Check back for more complex examples and features coming soon!

## Getting started

Make sure you've set up [Pigweed's
prerequisites](https://pigweed.dev/docs/getting_started.html#prerequisites).

**If you're on Windows**, you can automate the initial setup by downloading the
first-time setup script **from cmd.exe**:

```bat
curl https://pigweed.googlesource.com/pigweed/sample_project/+/main/tools/setup_windows_prerequisites.bat?format=TEXT > setup_pigweed_prerequisites.b64 && certutil -decode -f setup_pigweed_prerequisites.b64 setup_pigweed_prerequisites.bat && del setup_pigweed_prerequisites.b64
```

Then you can run the script with the following command **in cmd.exe**:

```bat
setup_pigweed_prerequisites.bat
```

Note: You may see a few UAC prompts as the script installs Git, Python, and
enables developer mode.

Once that is done, you can clone this project with the following command:

```sh
git clone https://pigweed.googlesource.com/pigweed/sample_project
```

### Environment setup

Pigweed uses a local development environment for most of its tools. This
means tools are not installed to your machine, and are instead stored in a
directory inside your project (Note: git ignores this directory). The tools
are temporarily added to the PATH of the current shell session.

To make sure the latest tooling has been fetched and set up, run the bootstrap
command for your operating system:

**Windows**

```bat
bootstrap.bat
```

**Linux & Mac**

```sh
source ./bootstrap.sh
```

After tooling updates, you might need to run bootstrap again to ensure the
latest tools.

After the initial bootstrap, you can use use the `activate` scripts to configure
the current shell for development without doing a full update.

**Windows**

```sh
activate.bat
```

**Linux & Mac**

```sh
source ./activate.sh
```

### Building

All of these commands must be run from inside an activated developer
environment. See [Environment setup](#environment-setup)

#### One-shot build

To build the project, documentation, and tests, run the following command in
an activated environment:

```sh
pw build
```

#### Automatically build on file save

Alternatively, if you'd like an automatic rebuild to trigger whenever you save
changes to files, use `pw watch`:

```sh
pw watch
```

## Typical workflow

When you pull latest repository changes, run bootstrap:
```sh
source ./bootstrap.sh
```

If you're just launching a new shell session, you can activate instead:
```sh
source ./activate.sh
```

and rebuild with:
```sh
pw build
```

## More info and Examples

Extended documentation and examples are built along code changes. You can view
them at `out/gn/docs/gen/docs/html/index.html`.
