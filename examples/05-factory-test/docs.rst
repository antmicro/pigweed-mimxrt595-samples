Factory Test Example
====================

Build and Flash
---------------

Instructions are the same as flashing
`blinky </workshop/01-blinky/README.md>`__ but passing in a different
``.elf``.

1. Run the compile with ``pw watch`` or ``ninja -C out``.

2. Flash ``factory-test.elf``.

   **Teensy**

   .. code:: sh

      arduino_unit_test_runner --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --verbose --flash-only out/arduino_debug/obj/workshop/05-factory-test/bin/factory_test.elf

   **stm32f429i_disc1**

   .. code:: sh

      openocd -s ${PW_PIGWEED_CIPD_INSTALL_DIR}/share/openocd/scripts -f ${PW_ROOT}/targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg -c "program out/stm32f429i_disc1_debug/obj/workshop/05-factory-test/bin/factory-test.elf reset exit"

View HDLC Encoded Log Output
----------------------------

1. **Optional:** Create / update the log token database. This will be
   automatically updated when compiling.

   .. code:: sh

      python -m pw_tokenizer.database create --force --database workshop/05-factory-test/tokenizer_database.csv out/arduino_debug/obj/workshop/05-factory-test/bin/factory-test.elf

2. Start the rpc_console that saves log output to a file.

   .. code:: sh

      python -m pw_hdlc.rpc_console -o logfile.txt -d /dev/ttyACM0 ./third_party/pigweed/pw_rpc/echo.proto

3. Tail the log output.

   .. code:: sh

      python -m pw_tokenizer.detokenize base64 workshop/05-factory-test/tokenizer_database.csv -i logfile.txt --follow

Running the Factory Test Script
-------------------------------

1. After flashing the factory-test.elf binary, execute the
   factory_test.py script.

   .. code:: sh

      python workshop/05-factory-test/factory_test.py --device /dev/ttyACM0 --baud 115200
