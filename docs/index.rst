.. _docs-root:

======================
Pigweed Sample Project
======================

.. toctree::
  :maxdepth: 1
  :hidden:

  Home <self>
  Examples <examples/docs>
  Customization <customization>
  Pigweed Docs <https://pigweed.dev>
  Mailing List <https://groups.google.com/forum/#!forum/pigweed>
  Chat Room <https://discord.gg/M9NSeTA>
  Source Code <https://cs.pigweed.dev/pigweed>
  Code Reviews <https://pigweed-review.googlesource.com>
  Issue Tracker <https://issues.pigweed.dev/issues?q=status:open>

This repository outlines the recommended way of using Pigweed in a new or
existing project. Feel free to fork this repository, or read it as a reference.

For more information see the `Pigweed Getting started
guide <https://pigweed.dev/docs/getting_started.html>`__.

Check back for more complex examples and features coming soon!

---------------
Getting started
---------------
Make sure you've set up `Pigweed's
prerequisites <https://pigweed.dev/docs/getting_started.html#prerequisites>`__.

**If you're on Windows**, you can automate the initial setup by downloading the
first-time setup script **from cmd.exe**:

.. code-block:: batch

   curl https://pigweed.googlesource.com/pigweed/sample_project/+/main/tools/setup_windows_prerequisites.bat?format=TEXT > setup_pigweed_prerequisites.b64 && certutil -decode -f setup_pigweed_prerequisites.b64 setup_pigweed_prerequisites.bat && del setup_pigweed_prerequisites.b64

Then you can run the script with the following command **in cmd.exe**:

.. code-block:: batch

   setup_pigweed_prerequisites.bat

.. note::

   You may see a few UAC prompts as the script installs Git, Python, and
   enables developer mode.

Once that is done, you can clone this project with the following command:

.. code-block:: bash

   git clone https://pigweed.googlesource.com/pigweed/sample_project

Environment setup
=================
Pigweed uses a local development environment for most of its tools. This
means tools are not installed to your machine, and are instead stored in a
directory inside your project (Note: git ignores this directory). The tools
are temporarily added to the PATH of the current shell session.

To make sure the latest tooling has been fetched and set up, run the bootstrap
command for your operating system:

**Windows**

.. code-block:: batch

   bootstrap.bat

**Linux & Mac**

.. code-block:: bash

   source ./bootstrap.sh

After tooling updates, you might need to run bootstrap again to ensure the
latest tools.

After the initial bootstrap, you can use use the ``activate`` scripts to
configure the current shell for development without doing a full update.

**Windows**

.. code-block:: bash

   activate.bat

**Linux & Mac**

.. code-block:: bash

   source ./activate.sh

.. _docs-building:

Building
========
All of these commands must be run from inside an activated developer
environment. See `Environment setup <#environment-setup>`__

One-shot build
--------------
To build the project, documentation, and tests, run the following command in
an activated environment:

.. code-block:: bash

   pw build

Automatically build on file save
--------------------------------
Alternatively, if you'd like an automatic rebuild to trigger whenever you save
changes to files, use ``pw watch``:

.. code-block:: bash

   pw watch

----------------
Typical workflow
----------------
When you pull latest repository changes, run bootstrap:

.. code-block:: bash

   source ./bootstrap.sh

If you're just launching a new shell session, you can activate instead:

.. code-block:: bash

   source ./activate.sh

and rebuild with:

.. code-block:: bash

   pw build

----------------------
More info and Examples
----------------------
Check out some introductory examples :ref:`here <examples-root>`!
