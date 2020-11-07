# Give Pigweed a Whirl Workshop

|||---|||

*** promo
#### [00: <br/> Setup](/workshop/README.md)

`Intro + setup.`
***

*** aside
#### [01: <br/> Blinky](/workshop/01-blinky/README.md)

`Getting to blink.`
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

[TOC]

## Intro

TBD...

## Repo Setup

Clone this repo with:

```sh
git clone --recursive https://pigweed.googlesource.com/pigweed/sample_project
```

No worries if you missed `--recursive`! Run `git submodules update --init` to pull `third_party/nanopb` and `third_party/pigweed`.

To pull new changes down run:

```sh
git pull --recurse-submodules
```

## Using GN

### Basics

* Create a build directory named `out`.

  ```sh
  gn gen out
  ```

* Set build options with `gn args`.

  ```sh
  gn args out
  ```

* Compile with

  ```sh
  ninja -C out
  ```

### Inspecting

* List buildable targets.

  ```sh
  gn ls out
  ```

* Inspect a target to see it's dependencies, `cflags`, `ldflags`, etc.

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

Use `--export-compile-commands` to create the `out/compile_commands.json` file for use with lsp servers like `clangd`.

```sh
gn gen out --export-compile-commands
```

`clangd` can be integrating with various text editor extensions such as:

* [VSCode clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
* [emacs lsp-mode](https://github.com/emacs-lsp/lsp-mode)
* [vim lsp](https://github.com/prabirshrestha/vim-lsp)

### Further Reading

* [GN Quick Start Guide](https://gn.googlesource.com/gn/+/master/docs/quick_start.md)
* [GN Reference](https://gn.googlesource.com/gn/+/master/docs/reference.md)
