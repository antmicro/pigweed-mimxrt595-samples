.. _examples-03-rpc:

========
03 - RPC
========
This example illustrates how to create and call RPCs using
`pw_rpc <https://pigweed.dev/pw_rpc/>`_ and
`pw_console <https://pigweed.dev/pw_console/>`_.

--------------------------
Trying out the RPC example
--------------------------

1. Build the test with ``pw build`` or ``pw watch``.

2. Flash ``test_runner_app.elf``.

   **STM32F429I_DISC1**

   .. code:: sh

      pw flash --device STM32-Discovery out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/03-rpc/bin/rpc_main.elf

   **Simulated device (Linux/macOS)**

   Start the simulated device in a separate terminal window with the following
   command:

   .. code::

      ./out/gn/host_device_simulator.speed_optimized/obj/examples/01-blinky/bin/blinky

3. Open `pw_console <https://pigweed.dev/pw_console/>`_.

   **Device**

   .. code:: sh

      pw console -d /dev/ttyACM0 -b 115200 --token-databases out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/03-rpc/bin/rpc_main.elf

   .. tip::

      On macOS, your device will look like ``/dev/cu.usbmodem2141403``, but
      will most likely end with a different number.

   **Simulated device (Linux/macOS)**

   .. code:: sh

     pw console -s default

4. In the ``Python Repl`` pane, use an RPC to request the device's UUID.

   .. code:: py

      device.rpcs.rpc_example.DeviceInfo.GetUuid()

   You should see it fail because the device does not yet have a UUID set:

   .. code::

      (Status.NOT_FOUND, rpc_example.DeviceUuid())

   In the ``Device Logs`` pane, you'll see a lot message like the following:

   .. code::

      UUID request received, but this device has no UUID yet

5. In the ``Python Repl`` pane, set the device's UUID.

   .. code:: py

      device.rpcs.rpc_example.DeviceInfo.SetUuid(uuid=b'\xab\xcd\ef\x01\x23\x45\x67\x89')

   It should succeed with the following response:

   .. code::

      (Status.OK, pw.protobuf.Empty())

6. In the ``Python Repl`` pane, use an RPC to request the device's UUID again.

   This time, the device should respond with the UUID you set using the
   previous command:

   .. code::

      (Status.OK, rpc_example.DeviceUuid(uuid=b'\xAB\xCD\xEF\x01\x23\x45\x67\x89'))

7. Try setting the UUID to a much longer string of bytes and see what happens!

8. When you're finished, you can type ``quit`` in the ``Python Repl`` pane to
   exit.
