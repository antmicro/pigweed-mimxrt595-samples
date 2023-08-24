Unit Tests & Strings
====================

Build and Flash
---------------

Instructions are the same as flashing
`blinky </workshop/01-blinky/README.md>`__ but passing in a different
``.elf``.

1. Run the compile with ``pw watch`` or ``ninja -C out``.

2. Flash the test ``.elf``

   .. code:: sh

      arduino_unit_test_runner --config out/arduino_debug/gen/arduino_builder_config.json --upload-tool teensyloader --verbose --flash-only out/arduino_debug/obj/workshop/02-string-functions/bin/string_demo.elf

3. Tail the output with ``miniterm``, (use ``Ctrl-]`` to quit).

   .. code:: sh

      python -m serial.tools.miniterm --raw - 115200

   You should see something like:

   .. code:: text

      $ python -m serial.tools.miniterm --raw - 115200
      --- Available ports:
      ---  1: /dev/ttyACM0         'USB Serial'
      --- Enter port index or full name: 1
      --- Miniterm on /dev/ttyACM0  115200,8,N,1 ---
      --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
      INF  [2020-11-08 14:15:20] Message number: 0
      INF  [2020-11-08 14:15:21] Message number: 1
      INF  [2020-11-08 14:15:22] Message number: 2
      INF  [2020-11-08 14:15:23] Message number: 3
      INF  [2020-11-08 14:15:24] Message number: 4
      INF  [2020-11-08 14:15:25] Message number: 5
      INF  [2020-11-08 14:15:26] Message number: 6
      INF  [2020-11-08 14:15:27] Message number: 7
      INF  [2020-11-08 14:15:28] Message number: 8

      --- exit ---

   All tests are set to use plain text logging. This is specified by the
   ``pw_log_BACKEND`` variable in the ``target_toolchain.gni`` files.
   For example the ``arduino_debug_tests`` toolchain in
   ```//targets/arduino/target_toolchains.gni`` </targets/arduino/target_toolchains.gni>`__
   defines: ``pw_log_BACKEND = "$dir_pw_log_basic"``

Exercise
--------

**Goal:** Write a test for the
```GetStatusString()`` </workshop/02-string-functions/system_status.cc#9>`__
function.

It can be added to
`/workshop/02-string-functions/system_status_test.cc </workshop/02-string-functions/system_status_test.cc>`__.

\**\* promo - Refer to the `gTest string comparison
documentation <https://github.com/google/googletest/blob/HEAD/docs/primer.md#string-comparison>`__
for how to check the contents of strings. - Note that `gMock
matchers <https://github.com/google/googletest/blob/HEAD/docs/advanced.md#more-string-assertions>`__
are not supported in Pigweed yet. \**\*

Runing Tests
------------

Run a Single Test Manually
~~~~~~~~~~~~~~~~~~~~~~~~~~

After compiling use ``arduino_unit_test_runner`` to flash and check test
output.

**Teensy**

.. code:: sh

   arduino_unit_test_runner --config out/arduino_debug_tests/gen/arduino_builder_config.json --upload-tool teensyloader --verbose out/arduino_debug_tests/obj/workshop/02-string-functions/test/system_status_test.elf

**stm32f429i_disc1**

.. code:: sh

   stm32f429i_disc1_unit_test_runner --verbose out/stm32f429i_disc1_debug/obj/workshop/02-string-functions/test/system_status_test.elf

Using the Test Server.
~~~~~~~~~~~~~~~~~~~~~~

1. Make sure the ``pw_arduino_use_test_server=true`` build arg is set.

2. Start the unit test server for your board.

   **Teensy**

   .. code:: sh

      arduino_test_server --verbose --config-file ./out/arduino_debug/gen/arduino_builder_config.json

   **stm32f429i_disc1**

   .. code:: sh

      stm32f429i_disc1_test_server --verbose

3. In a separate terminal start pw watch.

   .. code:: sh

      pw watch
