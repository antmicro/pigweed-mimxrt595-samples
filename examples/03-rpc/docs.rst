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

#. Build the test with ``pw build`` or ``pw watch``.

#. Launch the RPC example on a device, or using the simulated device.

   **STM32F429I_DISC1 (Linux/macOS)**

   * Flash ``rpc_main.elf``:

     .. code-block:: sh

        pw flash --device STM32-Discovery out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/03-rpc/bin/rpc_main.elf

     .. note::

        We don't yet have OpenOCD for Windows. See
        `b/300986008 <https://issues.pigweed.dev/300986008>`_ for updates.

   * Open `pw_console <https://pigweed.dev/pw_console/>`_.

     .. code-block:: sh

        pw console -d /dev/ttyACM0 -b 115200 --token-databases out/gn/stm32f429i_disc1_stm32cube.size_optimized/obj/examples/03-rpc/bin/rpc_main.elf

     .. tip::

        On macOS, your device will look like ``/dev/cu.usbmodem2141403``, but
        will most likely end with a different number.


   **Simulated device (all platforms)**

   Start the simulated device with the following command:

   .. code-block:: sh

      pw device-sim ./out/gn/host_device_simulator.speed_optimized/obj/examples/03-rpc/bin/rpc_main

#. In the ``Python Repl`` pane, use an RPC to request the device's UUID.

   .. code-block:: pycon

      >>> device.rpcs.rpc_example.DeviceInfo.GetUuid()

   You should see it fail because the device does not yet have a UUID set:

   .. code-block:: pycon

      (Status.NOT_FOUND, rpc_example.DeviceUuid())

   In the ``Device Logs`` pane, you'll see a log message like the following:

   .. code-block:: pycon

      UUID request received, but this device has no UUID yet

#. In the ``Python Repl`` pane, set the device's UUID.

   .. code-block:: pycon

      >>> device.rpcs.rpc_example.DeviceInfo.SetUuid(uuid=b'\xab\xcd\ef\x01\x23\x45\x67\x89')

   It should succeed with the following response:

   .. code-block:: pycon

      (Status.OK, pw.protobuf.Empty())

#. In the ``Python Repl`` pane, use an RPC to request the device's UUID again.

   This time, the device should respond with the UUID you set using the
   previous command:

   .. code-block:: pycon

      (Status.OK, rpc_example.DeviceUuid(uuid=b'\xAB\xCD\xEF\x01\x23\x45\x67\x89'))

#. Try setting the UUID to a much longer string of bytes and see what happens!

#. When you're finished, you can type ``quit`` in the ``Python Repl`` pane to
   exit.
