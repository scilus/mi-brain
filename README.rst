MI-Brain
========

Medical imaging application based on ``MITK 2026.06``, focused on diffusion MRI 
and tractography visualisation and interaction.

Based on code originally developed at `Imeka Solutions Inc.
<https://imeka.ca/>`_

Requirements
------------

See this `MITK page
<https://docs.mitk.org/2026.06/BuildInstructionsPage.html>`_ for the details.
MI-Brain has the exact same requirements as ``MITK 2026.06``.

How-to build
------------

Tu build MI-Brain, you must build MITK and link to the ``mi-brain`` folder in cmake.::

  cd mitk
  mkdir _build
  cd _build
  cmake -G "Ninja" \
      -D MITK_EXTENSION_DIRS=C:/path/to/mi-brain \
      -D MITK_BUILD_CONFIGURATION=WorkbenchReleaseNoPython \
      -D BUILD_TESTING:BOOL=OFF \
      ..

How-to package
--------------

#. On Linux and macOS, simply execute the ``PACKAGE`` target.
   The resulting redistributable archive will be located in your build
   directory::

    > ninja package

#. On Windows, in the Visual Studio 2022, right-click on the ``PACKAGE``
   project and ``Build``. The resulting redistributable archives
   (.exe and .zip) will be located in your build directory.
