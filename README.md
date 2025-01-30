# MIMXRT595-EVK Pigweed Samples

Copyright (c) 2024-2025 [Antmicro](https://www.antmicro.com)

This repository contains [Pigweed](https://pigweed.dev/) samples for the [MIMXRT595-EVK](https://www.nxp.com/design/design-center/development-boards-and-designs/i-mx-evaluation-and-development-boards/i-mx-rt595-evaluation-kit:MIMXRT595-EVK).
The repository structure is based on the [Pigweed sample project](https://pigweed.dev/examples/).

The following custom samples are provided:
- `examples/01_bluetooth` - Bluetooth advertiser demo, meant to show basic operation of `pw::bluetooth::Mimxrt595Controller`
- `examples/02_usb_fastboot` - Reference fastboot USB bootloader
- `examples/03_user_hello` - "Hello World" user application to boot from the bootloader
- `examples/04_bluetooth_audio` - A2DP Bluetooth audio sink, full-featured demo of `pw::bluetooth::Mimxrt595Controller` showcasing A2DP audio playback using on-board audio codec

## Environment setup

If you're setting up the local environment for the first time, you need to download the required tools. This can be done be sourcing the `bootstrap.sh` script.

```bash
source ./bootstrap.sh
```

After initial bootstrap is completed, you can use the `activate.sh` script to enter existing environment.

## Building samples

To build the samples you need to run `bazelisk` with the chosen sample target:

```bash
bazelisk build //examples/01_bluetooth:bluetooth.elf
bazelisk build //examples/02_usb_fastboot:usb_fastboot.elf
bazelisk build //examples/03_user_hello:user_hello.elf
bazelisk build //examples/04_bluetooth_audio:bluetooth_audio.elf
```

## Running samples

To run the samples, follow the [Running and debugging](https://pigweed.dev/targets/mimxrt595_evk_freertos/target_docs.html#running-and-debugging) section of the `mimxrt595-evk-freertos` Pigweed target documentation.
When connected to GDB, pass the desired sample executable to `file` command.
Sample executables are located under `bazel-bin/examples/<sample-name>/<target-name>.elf`.

```
>>> target remote :2331
Remote debugging using :2331
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x20081ad4 in ?? ()
>>> file bazel-bin/examples/01_bluetooth/bluetooth.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from bazel-bin/examples/01_bluetooth/bluetooth.elf...
>>> moni reset
Resetting target
>>> load
Loading section .flash_config, size 0x200 lma 0x8000400
Loading section .vector_table, size 0x168 lma 0x8001000
Loading section .code, size 0x17a3c8 lma 0x8001180
Loading section .ARM, size 0x8 lma 0x817b548
Loading section .static_init_ram, size 0x1124 lma 0x817b550
Start address 0x08003130, load size 1554524
Transfer rate: 46002 KB/sec, 15545 bytes/write.
>>> c
```

Debug output from samples is available on FLEXCOMM 12.

* Board UART RX (pin 1 on J27) - connect to TX pin on converter
* Board UART TX (pin 2 on J27) - connect to RX pin on converter
* Board GND (pin 7 on J29) - connect to GND pin on converter

## Running Bluetooth samples

`01_bluetooth` and `04_bluetooth_audio` are demo samples for showcasing the [`pw_bluetooth_sapphire_mcuxpresso`](https://github.com/antmicro/pigweed/tree/BT%2BUSB/pw_bluetooth_sapphire_mcuxpresso) Pigweed module.

Before running the samples, configure the board for Bluetooth:

1) Connect an `Embedded Artists 1XK M.2 Module (EAR00385)` module to the M.2 slot on the board
2) Ensure the following jumper configuration:
    * JP4 1-2
    * JP7 1-2
    * JP8 1-2
    * JP27 1-2
    * JP28 1-2
    * JP29 1-2
3) Connect J39 with external power

Run the sample by following steps in [Running samples](#running-samples).
You can verify the Bluetooth device is visible and connect to it by using `bluetoothctl` or using your desktop environment's Bluetooth control panel.

For audio sample, plug in a pair of headphones or speakers to the `LINE OUT` port of the board and connect to the `Pigweed A2DP Sink` Bluetooth device.
Play some audio on the host PC, and you should be able to hear it playing from the board.

## Running bootloader sample

`02_usb_fastboot` is a reference bootloader for RT595 that implements fastboot-over-USB to allow flashing the device at boot.
You must first [Build the sample](#building-samples), then follow the [Running samples](#running-samples) section to load the bootloader onto the device (executable will be located at `bazel-bin/examples/02_usb_fastboot/usb_fastboot.elf`).

At this point, the bootloader is flashed, but no application will be available for running yet.

### Flashing an application via fastboot

To jump into the fastboot bootloader to flash an application, **hold the SW1 button** located near the top of the board while resetting the board (either via dedicated **SW3 reset button**, or by power cycling the board).
You should be able to see the following output on FLEXCOMM12 UART:

```
INF  System init
INF  Registering RPC services
INF  Starting threads
INF  Running RPC server
INF  Fastboot bootloader: fastboot mode
INF  Ready to accept fastboot commands, connect to USB port J38..
```

At this point, you can connect the **Target USB** port (`J38`, bottom micro-USB port on the right) on the board to the host PC.
The device should be detected by the host, which you can verify by running `fastboot devices`:

```bash
sudo fastboot devices
# MIMXRT595-EVK    fastboot
```

To flash an application, you can run `fastboot flash`, with the partition name being `system` and the path pointing to one of the samples fastboot binaries.
All samples have an associated fastboot binary that should be used when flashing with fastboot (recognizable by the `.fastboot` suffix).

```bash
sudo fastboot flash system bazel-bin/examples/03_user_hello/user_hello.fastboot
# Sending sparse 'system' 1/2 (60 KB)                OKAY [  0.011s]
# Writing 'system'                                   OKAY [  0.451s]
# Sending sparse 'system' 2/2 (48 KB)                OKAY [  0.004s]
# Writing 'system'                                   OKAY [  0.361s]
# Finished. Total time: 0.837s
```

After flashing has completed successfully, you can reset the board as usual (without holding `SW1`) and the chosen sample will be booted (in this case the `03_user_hello` sample):

```
INF  System init
INF  Registering RPC services
INF  Starting threads
INF  Running RPC server
INF  Hello world from user app!
INF  This should be booted from an alternative flash location.
INF  Rebooting into fastboot in 4 seconds...
```

### Flashing bootloader via fastboot

You can also update the bootloader itself by providing the `fastboot` partition instead of `system` during `fastboot flash` invocation:

```bash
sudo fastboot flash fastboot bazel-bin/examples/02_usb_fastboot/usb_fastboot.fastboot
# Sending sparse 'fastboot' 1/3 (60 KB)              OKAY [  0.011s]
# Writing 'fastboot'                                 OKAY [  0.480s]
# Sending sparse 'fastboot' 2/3 (60 KB)              OKAY [  0.006s]
# Writing 'fastboot'                                 OKAY [  0.468s]
# Sending sparse 'fastboot' 3/3 (36 KB)              OKAY [  0.005s]
# Writing 'fastboot'                                 OKAY [  0.270s]
# Finished. Total time: 1.251s
```

Please note however that there is currently no redundancy for the bootloader, and flashing an invalid image will require reflashing using J-Link.
