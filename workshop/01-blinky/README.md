# Blinky with Logging

|||---|||

*** aside
#### [00: <br/> Setup](/workshop/README.md)

`Intro + setup.`
***

*** promo
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

## Demo: Build/Flash/Serial

Instructions for building with a `teensy40` board.

1. Create the `out` build directory.

   ```sh
   gn gen out --export-compile-commands --args='
       arduino_board="teensy40"
       dir_pw_third_party_arduino="//third_party/pigweed/third_party/arduino"
       arduino_core_name="teensy"
       pw_arduino_use_test_server=false
   '
   ```

1. Run the compile.

   ```sh
   ninja -C out
   ```

1. Flash `blinky.elf`.

   ```sh
   arduino_unit_test_runner \
     --config out/arduino_debug/gen/arduino_builder_config.json \
     --upload-tool teensyloader \
     --verbose \
     --flash-only \
     out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```

   **Single line:**

   ```sh
   arduino_unit_test_runner --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --verbose --flash-only out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```

1. Tail the output with `miniterm`, (use `Ctrl-]` to quit).

   *** note
   **Note:** This will only work when `pw_log_BACKEND = "$dir_pw_log_basic"`
   is set in `//targets/common_backends.gni`.
   ***

   ```sh
   python -m serial.tools.miniterm --raw - 115200
   ```

## Viewing Tokenized Log Output

1. **Optional:** Create / update the log token database. This will be automatically updated when compiling.

   ```sh
   python -m pw_tokenizer.database create --force \
     --database workshop/01-blinky/tokenizer_database.csv \
     out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```

   **Single line**

   ```sh
   python -m pw_tokenizer.database create --force --database workshop/01-blinky/tokenizer_database.csv out/arduino_debug/obj/workshop/01-blinky/bin/blinky.elf
   ```


1. Flash `blinky.elf` and watch the serial output with:

   *** note
   **Note:** This will only work when `pw_log_BACKEND = "$dir_pw_log_tokenized:log_backend"`
   is set in `//targets/common_backends.gni`.
   ***


   ```sh
   python -m pw_tokenizer.serial_detokenizer --device /dev/ttyACM0 --baudrate 115200 workshop/01-blinky/tokenizer_database.csv
   ```

## Viewing HLDC Encoded Log Output

1. Start the rpc_console that saves log output to a file.

   ```sh
   python -m pw_hdlc_lite.rpc_console -o logfile.txt -d /dev/ttyACM0 third_party/pigweed/pw_rpc/pw_rpc_protos/echo.proto
   ```

1. Tail the log output.

   **Linux & Mac**

   ```sh
   tail -F logfile.txt | python -m pw_tokenizer.detokenize base64 workshop/01-blinky/tokenizer_database.csv
   ```

   **Windows**

   ```sh
   python -m pw_tokenizer.detokenize base64 workshop/01-blinky/tokenizer_database.csv -i logfile.txt
   ```
