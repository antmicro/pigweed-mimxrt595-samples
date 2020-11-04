# String App & Unit Test

Instructions for building with a `teensy36` board.

1. Create the `out` build directory.

   ```sh
   gn gen out --export-compile-commands \
     --args='
       arduino_board="teensy36"
       dir_pw_third_party_arduino="//third_party/pigweed/third_party/arduino"
       arduino_core_name="teensy"
       pw_arduino_use_test_server=false'
   ```

1. Run the compile.

   ```sh
   ninja -C out
   ```

1. Flash.

   ```sh
   arduino_unit_test_runner \
     --config out/arduino_debug/gen/arduino_builder_config.json \
     --upload-tool teensyloader \
     --verbose \
     --flash-only \
     out/arduino_debug/obj/workshop/02-string-functions/bin/teensy_string_format.elf
   ```

1. Tail the output. If using plaintext logging using
   `pw_log_BACKEND = "$dir_pw_log_basic"` use `miniterm`, (use `Ctrl-]` to
   quit).

   ```sh
   python -m serial.tools.miniterm --raw - 115200
   ```

   You should see something like:

   ```text
   $ python -m serial.tools.miniterm --raw - 115200
   --- Available ports:
   ---  1: /dev/ttyACM0         'USB Serial'
   --- Enter port index or full name: 1
   --- Miniterm on /dev/ttyACM0  115200,8,N,1 ---
   --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
   INF  Teensy Time: RTC has set the system time.
   INF  Offs.  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  Text
   INF  0000: 53 75 70 65 72 20 53 69 6d 70 6c 65 20 54 69 6d  Super Simple Tim
   INF  0010: 65 20 4c 6f 67 67 69 6e 67 21 00                 e Logging!.
   INF  2020-11-04 14:35:35
   INF  2020-11-04 14:35:36
   INF  2020-11-04 14:35:37
   INF  2020-11-04 14:35:38
   INF  2020-11-04 14:35:39

   --- exit ---
   ```

1. Create tokenizer databases.

   ```sh
    python -m pw_tokenizer.database create --force \
      --database workshop/02-string-functions/teensy_tokenizer_database.csv \
      out/arduino_debug/obj/workshop/02-string-functions/bin/teensy_string_format.elf
   ```
