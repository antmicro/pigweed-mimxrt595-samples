# Give Pigweed a Whirl Workshop

|||---|||

*** promo
#### [00: <br/> Setup](/workshop/README.md)

`Intro + setup.`
***

*** aside
#### [01: <br/> Blinky](/workshop/01-blinky/README.md)

`Getting to blinky.`
***

*** aside
#### [02: <br/> Testing](/workshop/02-string-functions/README.md)

`Writing tests.`
***

*** aside
#### [03: <br/> RPC](/workshop/03-rpc/README.md)

`Calling RPCs.`
***

*** aside
#### [04: <br/> KVS](/workshop/04-kvs/README.md)

`Key Value Store.`
***

*** aside
#### [05: <br/> FactoryTest](/workshop/05-factory-test/README.md)

`Testing in the factory.`
***

|||---|||

![logo](/images/site_logo.png)

[TOC]

## Intro

Welcome! ...

## 1. Host Machine Setup

---

Python and Git are the only prerequisites for getting started with
Pigweed. Download and install if you don't already have them available.

**Windows**

1. Use the Windows installers for Python and Git from:

   - https://www.python.org/downloads/windows/
   - https://git-scm.com/download/win

   Make sure to add them to your system path during installation.

2. Enable long file paths enabled on Windows. This can be done using `regedit`
   or by running this as an administrator:

   ```bat
   REG ADD HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem /v LongPathsEnabled /t REG_DWORD /d 1
   ```

**Linux**

If you are using a [Teensy 3.x or
4.x](https://www.pjrc.com/teensy/td_download.html) board for the first time on a
Linux machine you will need to install the udev rules from:
https://www.pjrc.com/teensy/49-teensy.rules

**Mac**

Python and Git should be installed by default on Mac OS.

## 2. Repo Setup

---

Open a Terminal (`cmd.exe` on Windows) and clone this repo with:

```sh
git clone --recursive https://pigweed.googlesource.com/pigweed/sample_project
```

*** note
No worries if you missed `--recursive`! Run this to pull `third_party/nanopb`
and `third_party/pigweed`.

```sh
git submodules update --init
```

If you want to pull new changes run:

```sh
git pull --recurse-submodules
```
***

## 3. Run `bootstrap`

---

After cloning the build tools can be installed with the `bootstrap`
scripts. This is only required after the initial clone or updating Pigweed.

**Windows**

```sh
bootstrap.bat
```

**Linux & Mac**

```sh
. bootstrap.sh
```

After the initial bootstrap, use the `activate` scripts to setup your shell for
development.

**Windows**

```sh
activate.bat
```

**Linux & Mac**

```sh
. activate.sh
```

## 4. Install Teensyduino Core

---

To build for Arduino boards you must install a core. At this time only the
[Teensyduino core](https://www.pjrc.com/teensy/td_download.html) is
supported. Check the Pigweed [Arduino target
docs](https://pigweed.dev/targets/arduino/target_docs.html) for more info.

All Arduino cores should be installed into
`third_party/pigweed/third_party/arduino/cores/`

Run this to install the Teensy core:

```sh
arduino_builder install-core --prefix third_party/pigweed/third_party/arduino/cores/ --core-name teensy
```

## 5. Build!

---

### STM32F429i Discovery Board

To build for the `stm32f429i_disc1` board run:

```sh
gn gen out
```

Then start the compile with:

```sh
ninja -C out
```


### Teensy 3.x/4.x

To build for a Teensy 4.0 board run the following.

**Windows**

Run `gn args out` which will open a text editor. Paste in the following, save and
close the editor.

```sh
dir_pw_third_party_arduino="//third_party/pigweed/third_party/arduino"
arduino_core_name="teensy"
arduino_board="teensy40"
arduino_menu_options=["menu.usb.serial", "menu.keys.en-us"]
pw_arduino_use_test_server=false
```

The `arduino_board` arg can be set to any of these:

- `"teensy31"` - Teensy 3.2 / 3.1
- `"teensy35"` - Teensy 3.5
- `"teensy36"` - Teensy 3.6
- `"teensy40"` - Teensy 4.0
- `"teensy41"` - Teensy 4.1

After `gn` is done, compile everything with:

```sh
ninja -C out
```

**Linux & Mac**

You can use `gn args out` as shown above or include the args on the command line:

```sh
gn gen out --args='
  dir_pw_third_party_arduino="//third_party/pigweed/third_party/arduino"
  arduino_core_name="teensy"
  arduino_board="teensy40"
  arduino_menu_options=["menu.usb.serial", "menu.keys.en-us"]
  pw_arduino_use_test_server=false
'
```

After `gn` is done, compile everything with:

```sh
ninja -C out
```

## GN and Ninja Reference

---

### Basics

- Create a build directory named `out`.

  ```sh
  gn gen out
  ```

- Set build options with `gn args`.

  ```sh
  gn args out
  ```

- Compile with

  ```sh
  ninja -C out
  ```

- Clean by deleting the out folder or running:

  ```sh
  ninja -C out -t clean
  ```


### Inspecting

- List buildable targets.

  ```sh
  gn ls out
  ```

- Inspect a target to see it's dependencies. E.g. `cflags`, `ldflags`, etc.

  **Teensy**
  ```sh
  gn desc out "//workshop/01-blinky:blinky(//targets/arduino:arduino_debug)" --tree
  ```

  **stm32f429i_disc1**
  ```sh
  gn desc out "//workshop/01-blinky:blinky(//targets/stm32f429i-disc1:stm32f429i_disc1_debug)" --tree
  ```

  **Host**
  ```sh
  gn desc out "//workshop/01-blinky:blinky(//targets/host:host_debug)" --tree
  ```

### Editor Integration

Use `--export-compile-commands` to create the `out/compile_commands.json` file
for use with lsp servers like `clangd`.

```sh
gn gen out --export-compile-commands
```

`clangd` can be integrating with various text editor extensions such as:

- [VSCode clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
- [emacs lsp-mode](https://github.com/emacs-lsp/lsp-mode)
- [vim lsp](https://github.com/prabirshrestha/vim-lsp)

### Further Reading

- [GN Quick Start Guide](https://gn.googlesource.com/gn/+/master/docs/quick_start.md)
- [GN Reference](https://gn.googlesource.com/gn/+/master/docs/reference.md)
