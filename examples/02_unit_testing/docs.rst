.. _examples-02-unit-testing:

=================
02 - Unit testing
=================
This example illustrates how to create and run on-device unit tests. There are
many ways you can integrate Pigweed's
`pw_unit_test <https://pigweed.dev/pw_unit_test/>`_ module with your project to
provide easy, automated unit testing. This example uses a test runner that sends
tests results over `pw_rpc <https://pigweed.dev/pw_rpc/>`_.

----------------------------
Running the test on a device
----------------------------

#. Build the test with ``pw build`` or ``pw watch``.

#. Flash ``test_runner_app.elf``.

   **STM32F429I_DISC1 (Linux/MacOS)**

   .. code-block:: sh

      pw flash --device STM32-Discovery out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/02_unit_testing/bin/test_runner_app.elf

   .. note::
      We don't yet have OpenOCD for Windows. See
      `b/300986008 <https://issues.pigweed.dev/300986008>`_ for updates.

   **Raspberry Pi Pico (RP2404) (Windows/Linux/MacOS)**

   1. Reboot the Pico into BOOTSEL mode by holding the bootsel button when
      plugging into USB.
   2. Copy ``./out/gn/rp2040.size_optimized/obj/examples/02_unit_testing/test_runner_app.uf2``
      to your Pi Pico.

   .. note::
      It is also possible to flash a Pico board with `picotool
      <https://github.com/raspberrypi/picotool>`_. We will be adding support for
      that in this repo soon. See `b/300321451
      <https://issues.pigweed.dev/300321451>`_ for updates.

#. Open `pw_console <https://pigweed.dev/pw_console/>`_.

   **Device**

   .. code-block:: sh

      pw console -d /dev/ttyACM0 -b 115200 --token-databases out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/02_unit_testing/bin/test_runner_app.elf

   .. tip::

      On macOS, your device will look like ``/dev/cu.usbmodem2141403``, but
      will most likely end with a different number.

#. In the ``Python Repl`` pane, run the tests.

   .. code-block:: pycon

      >>> device.run_tests()

#. When you're finished, you can type ``quit`` in the ``Python Repl`` pane to
   exit.

--------------------------------------
Running the test in a simulated device
--------------------------------------

#. Build the test with ``pw build`` or ``pw watch``.

#. Launch ``test_runner_app`` using the ``pw device-sim`` helper.

   .. code-block::

      pw device-sim --sim-binary ./out/gn/host_device_simulator.speed_optimized/obj/examples/02_unit_testing/bin/test_runner_app

#. In the ``Python Repl`` pane, run the tests.

   .. code-block:: pycon

      >>> device.run_tests()

#. When you're finished, you can type ``quit`` in the ``Python Repl`` pane to
   exit.

----------------------
Fixing the broken test
----------------------
To make debugging the test faster, add the failing test to the root
build file at ``//BUILD.gn``:

.. code-block::

   # All the tests that should run as part of the build.
   pw_test_group("tests") {
     group_deps = [
       "//examples/02_unit_testing:tests",
       # ...
     ]
   }

You'll notice that if you run ``pw build`` again, this time the test failure
will appear as part of the build:

.. code-block::

   $ pw build
       ╱╱╱╱  █▀█ █ █▀▀ █░█░█ █▀▀ █▀▀ █▀▄
      ╱╱╱╱╱  █▀▀ █ █▄█ ▀▄▀▄▀ ██▄ ██▄ █▄▀
     ╱╱╱╱╱╱
     ╱╱╱╱╱╱     █▀ ▄▀█ █▀▄▀█ █▀█ █░░ █▀▀
     ╱╱╱╱╱╱     ▄█ █▀█ █░▀░█ █▀▀ █▄▄ ██▄
     ╱╱╱╱╱╱
     ╱╱╱╱╱       █▀█ █▀█ █▀█ ░░█ █▀▀ █▀▀ ▀█▀
     ╱╱╱╱        █▀▀ █▀▄ █▄█ █▄█ ██▄ █▄▄ ░█░

   16:38:01 INF Checking all files in the sample_project repo
   16:38:01 INF Starting build with 1 directories
   16:38:01 INF Root logfile: /Users/amontanez/development/projects/pigweed/sample_project/out/build.txt
   16:38:01 INF [1/1] Starting ==> Recipe: default Targets: default Logfile: /Users/amontanez/development/projects/pigweed/sample_project/out/build_default.txt
   16:38:01 INF [1/1] Skipped ==> gn gen /Users/amontanez/development/projects/pigweed/sample_project/out/gn --export-compile-commands
   16:38:01 INF [1/1] Run ==> ninja -C out/gn default
   16:38:02 INF
   16:38:02 INF ninja: Entering directory `out/gn'
   16:38:04 ERR FAILED: host_debug_tests/gen/examples/02_unit_testing/bitops_test._run.pw_pystamp
   16:38:04 INF python ../../third_party/pigweed/pw_build/py/pw_build/python_runner.py --gn-root ../../ --current-path ../../examples/02_unit_testing --default-toolchain=//third_party/pigweed/pw_toolchain/default:default --current-toolchain=//targets/host:host_debug_tests --touch host_debug_tests/gen/examples/02_unit_testing/bitops_test._run.pw_pystamp --capture-output --module pw_unit_test.test_runner --python-virtualenv-config python/gen/sample_project_build_venv/venv_metadata.json --python-dep-list-files host_debug_tests/gen/examples/02_unit_testing/bitops_test._run_metadata_path_list.txt -- --runner ../../third_party/pigweed/targets/host/run_test --test \<TARGET_FILE\(:bitops_test\)\>
   16:38:04 INF ERR ../../third_party/pigweed/targets/host/run_test exited with status 1
   16:38:04 INF OUT [Pid: 16631]
   16:38:04 INF INF  [==========] Running all tests.
   16:38:04 INF INF  [ RUN      ] Bitops.SomeOnes
   16:38:04 INF INF  [       OK ] Bitops.SomeOnes
   16:38:04 INF INF  [ RUN      ] Bitops.MoreOnes
   16:38:04 INF INF  [       OK ] Bitops.MoreOnes
   16:38:04 INF INF  [ RUN      ] Bitops.EvenMoreOnes
   16:38:04 INF ERR  examples/02_unit_testing/bitops_test.cc:26: Failure
   16:38:04 INF ERR        Expected: CountOnes(0b11100101) == 5
   16:38:04 INF ERR          Actual: 2 == 5
   16:38:04 INF ERR  [  FAILED  ] Bitops.EvenMoreOnes
   16:38:04 INF INF  [ RUN      ] Bitops.NoOnes
   16:38:04 INF INF  [       OK ] Bitops.NoOnes
   16:38:04 INF INF  [==========] Done running all tests.
   16:38:04 INF INF  [  PASSED  ] 3 test(s).
   16:38:04 INF ERR  [  FAILED  ] 1 test(s).
   16:38:04 INF INF Test 1/1: [FAIL] bitops_test in 0.029 s
   16:38:04 INF ninja: build stopped: subcommand failed.
   16:38:04 INF
   16:38:04 ERR [1/1] Finished ==> Recipe: default (FAIL)
   16:38:04 INF
   16:38:04 INF    ▄██████▒░▄▄▄       ██▓  ░██▓
   16:38:04 INF   ▓█▓     ░▒████▄    ▓██▒  ░▓██▒
   16:38:04 INF   ▒████▒   ░▒█▀  ▀█▄  ▒██▒ ▒██░
   16:38:04 INF   ░▓█▒    ░░██▄▄▄▄██ ░██░  ▒██░
   16:38:04 INF   ░▒█░      ▓█   ▓██▒░██░░ ████████▒
   16:38:04 INF    ▒█░      ▒▒   ▓▒█░░▓  ░  ▒░▓  ░
   16:38:04 INF    ░▒        ▒   ▒▒ ░ ▒ ░░  ░ ▒  ░
   16:38:04 INF    ░ ░       ░   ▒    ▒ ░   ░ ░
   16:38:04 INF                  ░  ░ ░       ░  ░
   ━━ FAILED (1) ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    default FAIL     ninja: build stopped: subcommand failed.

Now you can fix the broken test without having to wait for a device to flash!
Once you've fixed the test, try running it on the device again to verify your
fix is portable.
