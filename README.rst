MI-Brain
========

Medical imaging application based on MITK 2025.10, focused on diffusion MRI 
and tractography visualisation and interaction.

Based on code originally developed at Imeka Solutions Inc.

**Current Status**: MITK 2025 migration in progress on ``update-mitk-2025`` branch.

Known Issues
------------

- **Fiber visibility**: TRK files load successfully but fibers remain invisible when 
  loaded with anatomy. Rendering pipeline is functional (confirmed via debugging), 
  but unknown issue prevents display. See ``MITK_2025_FIBER_VISIBILITY_FIXES.md`` 
  for details on attempted fixes.

Requirements
------------

- `CMake
  <http://www.cmake.org/>`_ >= 3.18
- `Qt
<<<<<<< Updated upstream
  <https://download.qt.io/archive/qt/>`_ == 6.5.x
- `MITK`_ == 2025.10 (with VTK 9.4)
=======
  <https://download.qt.io/archive/qt/>`_ >= 6.5.x
- `MITK <https://github.com/MITK/MITK>`_ == 25.12
>>>>>>> Stashed changes

How-to build
------------

Build instructions for MITK 2025 migration:

#. Build MITK 2025.10 with VTK 9.4 following MITK's official build instructions.

#. Configure MI-Brain build::

    > mkdir build-2025 && cd build-2025
    cmake -DMITK_DIR=/path/to/MITK-2025-build ..

**Note:** If CMake struggles to find Qt6, you might need to explicitly provide the path to your Qt6 CMake configuration, like so:

.. code-block:: bash

    cmake -DQt6_DIR:PATH=/home/local/USHERBROOKE/rhef1902/Libraries/Qt/6.10.2/gcc_64/lib/cmake/Qt6 -DMITK_DIR=/home/local/USHERBROOKE/rhef1902/Libraries/MITK/MITK-superbuild/MITK-build ..

#. Build MI-Brain::

    > cmake --build . -j8

The built application will be in ``build-2025/MI-Brain-<version>-<platform>/``.

**Note**: Legacy MITK 2018 build instructions have been superseded by the above.

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
