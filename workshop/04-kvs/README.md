# KVS with Logging

Instructions for building with a `teensy40` board.

Create the `out` build directory.

```sh
gn gen out --export-compile-commands \
  --args='
    arduino_board="teensy40"
    dir_pw_third_party_arduino="//third_party/pigweed/third_party/arduino"
    arduino_core_name="teensy"
    pw_arduino_use_test_server=false'
```

Run the compile.

```sh
ninja -C out
```

Flash `kvs.elf`.

```sh
arduino_unit_test_runner \
  --config out/arduino_debug/gen/arduino_builder_config.json \
  --upload-tool teensyloader \
  --verbose \
  --flash-only \
  out/arduino_debug/obj/workshop/04-kvs/bin/kvs.elf
```

Tail the output with `miniterm`, (use `Ctrl-]` to quit).

```sh
python -m serial.tools.miniterm --raw - 115200
```

## Viewing Tokenized Log Output

After compiling create/update the log token database.

```sh
python -m pw_tokenizer.database create --force \
  --database workshop/04-kvs/tokenizer_database.csv \
  out/arduino_debug/obj/workshop/04-kvs/kvs.main.cc.o
```

Flash `kvs.elf` and watch the serial output with:

*** note
This produces no output until hitting Ctrl-C.
***

```sh
python -m pw_tokenizer.serial_detokenizer \
  --device /dev/ttyACM0 --baudrate 115200 \
  workshop/04-kvs/tokenizer_database.csv
```

## Viewing HLDC Encoded Log Output

*** note
This section WIP
***

```sh
python -m pw_hdlc_lite.rpc_console -o logfile.txt -d /dev/ttyACM0 \
  ./third_party/pigweed/pw_rpc/pw_rpc_protos/echo.proto
```

```sh
tail -F logfile.txt | python -m pw_tokenizer.detokenize \
  base64 workshop/04-kvs/tokenizer_database.csv
```
