RPC
===

Build and Flash
---------------

Instructions are the same as flashing
`blinky </workshop/01-blinky/README.md>`__ but passing in a different
``.elf``.

1. Run the compile with ``pw watch`` or ``ninja -C out``.

2. Flash ``rpc.elf``.

   **Teensy**

   .. code:: sh

      arduino_unit_test_runner --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --verbose --flash-only out/arduino_debug/obj/workshop/03-rpc/bin/rpc.elf

   **stm32f429i_disc1**

   .. code:: sh

      openocd -s ${PW_PIGWEED_CIPD_INSTALL_DIR}/share/openocd/scripts -f ${PW_ROOT}/targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg -c "program out/arduino_debug/obj/workshop/03-rpc/bin/rpc.elf reset exit"

View HDLC Encoded Log Output
----------------------------

1. **Optional:** Create / update the log token database. This will be
   automatically updated when compiling.

   .. code:: sh

      python -m pw_tokenizer.database create --force --database workshop/03-rpc/tokenizer_database.csv out/arduino_debug/obj/workshop/03-rpc/bin/rpc.elf

2. Start the rpc_console that saves log output to a file.

   .. code:: sh

      python -m pw_hdlc.rpc_console -o logfile.txt -d /dev/ttyACM0 ./third_party/pigweed/pw_rpc/echo.proto workshop/03-rpc/remoticon_proto/remoticon.proto

   This will launch an interactive ``ipython`` console where you can
   send RPCs. Try ``rpcs.pw.rpc.EchoService.Echo(msg="hello!")``.
   IPython provides nice tab completion for the RPC interfaces as well.

3. Tail the log output.

   .. code:: sh

      python -m pw_tokenizer.detokenize base64 workshop/03-rpc/tokenizer_database.csv -i logfile.txt --follow
