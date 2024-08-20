# MIMXRT595-EVK Pigweed Samples

Copyright (c) 2024 [Antmicro](https://www.antmicro.com)

This repository contains [Pigweed](https://pigweed.dev/) samples for the [MIMXRT595-EVK](https://www.nxp.com/design/design-center/development-boards-and-designs/i-mx-evaluation-and-development-boards/i-mx-rt595-evaluation-kit:MIMXRT595-EVK).
The repository structure is based on the [Pigweed sample project](https://pigweed.dev/examples/).

The following, custom, samples are provided:
- `examples/01_bluetooth`
- `examples/02_usb_fastboot`

## Environment setup

If you're setitng local environment for the first time, you need to download the required tools. This can be done be sourcing the `bootstrap.sh` script.

```bash
source ./bootstrap.sh
```

After initial bootstrap is completed, you can use the `activate.sh` script to enter existing environment.

## Building samples

Before building the samples, you will need to download NXP's [MCUXpresso SDK](https://mcuxpresso.nxp.com/en/welcome) for your device and unpack it to the `third_party/pigweed/third_party/mcuxpresso/sdk` folder.

To build the samples you need to generate build rules and run `ninja` in the generated folder:

```bash
gn gen out
```

Once the build rules are in place, you can build the samples with:
```bash
ninja -C out
```
