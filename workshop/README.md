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

git clone...

## Using GN

Create build directory
```sh
gn gen out
```

Set build options with args

```sh
gn args out
```

List buildable targets

```sh
gn ls out
```

Inspect a target to see dependencies / cflags etc

```sh
gn desc out "//workshop/01-blinky:blinky(//targets/arduino:arduino_debug)" --tree
```

```sh
gn desc out "//workshop/01-blinky:blinky(//targets/stm32f429i-disc1:stm32f429i_disc1_debug)" --tree
```

Advanced: `use gn gen out --export-compile-commands` for lsp server integration.
