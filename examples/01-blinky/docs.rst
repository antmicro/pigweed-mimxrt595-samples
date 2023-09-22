.. _examples-01-blinky:

========================
01 - Blinky with logging
========================
Blinky introduces the basics of building, flashing and checking log
output. The instructions here will be the same for each exercise and
binary to be flashed.

Rather than sitting in a blocking loop to blink an LED, this implementation uses
timers to allow the device to do other work in between blinks.

---------------
Build and flash
---------------

1. Build the firmware with ``pw build`` or ``pw watch``.

2. Flash ``blinky.elf``.

   **STM32F429I_DISC1 (Linux/macOS)**

   .. code-block:: sh

      pw flash --device STM32-Discovery out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/01-blinky/bin/blinky.elf

   .. note::

      We don't yet have OpenOCD for Windows. See
      `b/300986008 <https://issues.pigweed.dev/300986008>`_ for updates.

   **Simulated device (all platforms)**

   Rather than blinking an LED, this will just emit logs to mimic LED state.
   You can see the logs by attaching the console. Start the simulated device
   in a separate terminal window with the following command:

   .. code-block::

      ./out/gn/host_device_simulator.speed_optimized/obj/examples/01-blinky/bin/blinky

---------
View logs
---------
Logs are `tokenized <https://pigweed.dev/pw_tokenizer/>`_ and transmitted using
`pw_rpc <https://pigweed.dev/pw_rpc/>`_. If you try to view the UART output
using a serial terminal like ``minicom`` or ``screen``, the device's output
will not be human-readable because the device is sending machine-readable binary
data.

`pw_console <https://pigweed.dev/pw_console/>`_ can take this binary data and
turn it into human-readable logs and bidirectional RPC commands.

You can view the logs from your attached device with the following command:

**Device**

.. code-block:: sh

   pw console -d /dev/ttyACM0 -b 115200 --token-databases out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/01-blinky/bin/blinky.elf

.. tip::

   On macOS, your device will look like ``/dev/cu.usbmodem2141403``, but
   will most likely end with a different number.

**Simulated device (Linux/macOS)**

.. code-block:: sh

  pw console -s default
