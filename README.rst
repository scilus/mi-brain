MI-Brain
========

Medical imaging application based on MITK 2018.04, focused on diffusion MRI 
and tractography visualisation and interaction.

Based on code originally developed at Imeka Solutions Inc.

Requirements
------------

- `CMake
  <http://www.cmake.org/>`_ == 3.12.x or 3.13.x
- `Qt
  <https://download.qt.io/archive/qt/>`_ == 5.12.x up to 5.12.5 excluded
- (recommended) `MITK`_ == dev2018

How-to build
------------

You must build the MITK's development branch before building this project.
The documentation to build MITK and this project (MI-Brain) can be found in
those documents. Contact Imeka if needed.

How-to run the tests
--------------------

#. On Linux and macOS, simply run this command with the desired number of
   processors::

    > ctest -j #

#. On Windows, in the Visual Studio 2017, right-click on the ``RUN_TEST``
   project and ``Build``. It will run on only one processor. If you want to run
   the test on a specific number of processors, open the a command line, copy
   the path line from the ``startMitkWorkbench_release.bat`` in your
   ``MITK-build\\bin`` folder, then run::
   
    > PATH=.............;%PATH%
    > ctest -j #

How-to package
--------------

#. On Linux and macOS, simply execute the ``PACKAGE`` target.
   The resulting redistributable archive will be located in your build
   directory::

    > make package

#. On Windows, in the Visual Studio 2017, right-click on the ``PACKAGE``
   project and ``Build``. The resulting redistributable archives
   (.exe and .zip) will be located in your build directory.