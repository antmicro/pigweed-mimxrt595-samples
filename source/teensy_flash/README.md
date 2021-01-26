# Teensy Flash Example

Instructions for building with a `teensy40` board.

[TOC]

## Build/Flash/Run

1. Create the `out` build directory.

   ```sh
   gn gen out --export-compile-commands --args='
       pw_arduino_build_CORE_PATH="//third_party/pigweed/third_party/arduino/cores"
       pw_arduino_build_CORE_NAME="teensy"
       pw_arduino_build_PACKAGE_NAME = "teensy/avr"
       pw_arduino_build_BOARD="teensy40"
       pw_arduino_use_test_server=false'
   ```

1. Patch arduino core.

   Running the following command after installing the arduino core:

   ```
   sed -i -e 's/^static void flash_/void flash_/' third_party/pigweed/third_party/arduino/cores/teensy/hardware/teensy/avr/cores/teensy4/eeprom.c
   ```

1. Run the compile.

   ```sh
   ninja -C out
   ```

1. Flash and run test.

   ```sh
   arduino_unit_test_runner --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --verbose out/arduino_debug_tests/obj/source/teensy_flash/test/teensy_flash_test.elf
   ```
   You should see something somewhat similar to:

   ```sh
   DBG Successfully flashed firmware to device
   INF [==========] Running all tests.
   INF [ RUN      ] FlashPartitionTest.FillTest
   INF DBG  FillTest iteration 0, write '0'
   INF DBG  FillTest iteration 0, write '0xff'
   INF DBG  FillTest iteration 0, write '0x55'
   INF DBG  FillTest iteration 0, write '0xa3'
   INF DBG  Completed iterations 0, Total errors 0
   INF DBG  FillTest iteration 1, write '0'
   INF DBG  FillTest iteration 1, write '0xff'
   INF DBG  FillTest iteration 1, write '0x55'
   INF DBG  FillTest iteration 1, write '0xa3'
   INF DBG  Completed iterations 1, Total errors 0
   INF [       OK ] FlashPartitionTest.FillTest
   INF [ RUN      ] FlashPartitionTest.EraseTest
   INF [       OK ] FlashPartitionTest.EraseTest
   INF [ RUN      ] FlashPartitionTest.AlignmentCheck
   INF [       OK ] FlashPartitionTest.AlignmentCheck
   INF [ RUN      ] FlashPartitionTest.IsErased
   DBG [       OK ] EmptyInitializedKvs.Basic
   DBG [==========] Done running all tests.
   DBG [  PASSED  ] 4 test(s).
   INF Test passed!
   ```

