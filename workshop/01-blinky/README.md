# Blinky with Logging

|||---|||

*** aside
#### [00: <br/> Setup](/workshop/README.md)

`Intro + setup.`
***

*** promo
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

[TOC]

## Goal

Blinky introduces the basics of building, flashing and checking log output. The
instructions here will be the same for each exercise and binary to be flashed.

## Build and Flash

Instructions for building with a `teensy40` board.

1. Create the `out` build directory if needed.

   ```sh
   gn gen out --export-compile-commands --args='
       pw_arduino_build_BOARD="teensy40"
       pw_arduino_build_CORE_PATH="//third_party/pigweed/third_party/arduino"
       pw_arduino_build_CORE_NAME="teensy"
       pw_arduino_use_test_server=false
   '
   ```

1. Run the compile with `pw watch out` or `ninja -C out`.

1. Flash `blinky.elf`.

   **Teensy**

   ```sh
   arduino_unit_test_runner --verbose --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --flash-only out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```

   **stm32f429i_disc1**

   ```sh
   openocd -s ${PW_PIGWEED_CIPD_INSTALL_DIR}/share/openocd/scripts -f ${PW_ROOT}/targets/stm32f429i-disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg -c "program out/stm32f429i_disc1_debug/obj/workshop/01-blinky/bin/blinky.elf reset exit"
   ```

## View Plain Text Log Output

*** note
**Note:** This will only work when `pw_log_BACKEND = "$dir_pw_log_basic"` is set
in `//targets/common_backends.gni`. The default is set to use tokenized logging
over HDLC as shown below.
***

Tail the output with `miniterm`, (use `Ctrl-]` to quit).

```sh
python -m serial.tools.miniterm --raw - 115200
```

## View HDLC Encoded Log Output

1. **Optional:** Create / update the log token database. This will be automatically updated when compiling.

   ```sh
   python -m pw_tokenizer.database create --force --database workshop/01-blinky/tokenizer_database.csv out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```

1. Start the rpc_console that saves log output to a file.

   ```sh
   python -m pw_hdlc_lite.rpc_console -o logfile.txt -d /dev/ttyACM0 third_party/pigweed/pw_rpc/pw_rpc_protos/echo.proto
   ```

1. Tail the log output.

   ```sh
   python -m pw_tokenizer.detokenize base64 workshop/01-blinky/tokenizer_database.csv -i logfile.txt --follow
   ```
