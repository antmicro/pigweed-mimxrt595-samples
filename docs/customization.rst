.. _docs-customization:

=============
Customization
=============
If you're using this repository as a base for your own project, you can
customize a variety of aspects to meet your needs.

------
Banner
------
Make the project yours with your own banner! Edit ``banner.txt`` as desired,
you can find some suggestions for generating art
`here <https://pigweed.dev/pw_cli/#making-the-ascii-ansi-art>`_.

----------
Presubmits
----------
Sample presubmit checks are included in ``tools/presubmit_checks.py``.
To install them as a pre-push git hook, run ``pw presubmit --install``. See
the `pw_presubmit <https://pigweed.dev/pw_presubmit>`_ module documentation for
more information.


------------------
Sample application
------------------
The sample application in ``source/main.cc`` uses the sample module
``simple_counter``. Look at ``source/BUILD.gn`` and
``source/simple_counter/BUILD.gn`` to see how these are built respectively. The
key part is in the root ``BUILD.gn``, which creates the host target using the
host toolchain. A toolchain is required for each target.

:ref:`Build <docs-building>` the project and then run the test with the command

command below.

```sh
./out/host_clang_debug/obj/source/bin/hello_world
```

-----------
Sample test
-----------
The ``simple_counter`` module has tests defined in
``source/simple_counter_tests.cc``. Look at ``source/simple_counter/BUILD.gn``
for an example of how a test is defined. The root ``BUILD.gn`` groups all the
host tests together.

:ref:`Build <docs-building>` the project and run the test with the command
below.

```sh
./out/host_clang_debug_tests/obj/source/simple_counter/test/simple_counter_test
```
