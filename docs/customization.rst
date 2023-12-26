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
