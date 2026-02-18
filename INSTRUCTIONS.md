# MI-Brain Build Instructions

These instructions describe how to build the MI-Brain project.

## Requirements

*   **CMake**: Version 3.18 or higher.
*   **Qt**: Version 6.5.x is recommended in the `README.rst`, but version 6.10.1 was found and used for these instructions. The build system will look for it in `/home/local/USHERBROOKE/rhef1902/Libraries/Qt/`.
*   **MITK**: Version 2025.10. The build is configured to use the MITK build located at `/home/local/USHERBROOKE/rhef1902/Libraries/MITK-2025.12/MITK-superbuild/MITK-build`.

### Optional Dependencies

During the CMake configuration, the following optional dependencies might be reported as missing:

*   **FreeType**: Used for font rendering. If it's missing, some text might not be rendered correctly. You can install it using your system's package manager. For example, on Debian/Ubuntu:
    ```bash
    sudo apt-get install libfreetype6-dev
    ```

*   **Doxygen**: Used for generating documentation. It's not required to build the application. If you want to generate the documentation, you can install it using your system's package manager. For example, on Debian/Ubuntu:
    ```bash
    sudo apt-get install doxygen
    ```

## Build Steps

1.  **Create and navigate to the build directory:**
    ```bash
    mkdir -p build-2025
    cd build-2025
    ```

2.  **Configure the project with CMake:**
    Run the following command from within the `build-2025` directory. This command points to the MITK and Qt6 dependency locations.

    ```bash
    cmake -DMITK_DIR=/home/local/USHERBROOKE/rhef1902/Libraries/MITK-2025.12/MITK-superbuild/MITK-build -DQt6_DIR=/home/local/USHERBROOKE/rhef1902/Libraries/Qt/6.10.1/gcc_64/lib/cmake/Qt6 ..
    ```

3.  **Build the project:**
    After CMake has finished configuring, you can build the project using the following command. The `-j` flag specifies the number of parallel jobs to use for building. Adjust it based on your system's capabilities.

    ```bash
    cmake --build . -j8
    ```

4.  **Run the application:**
    The compiled application will be located in the `build-2025/bin` directory. 
    A script `run_mi_brain.sh` is available in the root of the project to run the application with the correct environment variables.
    You might need to adapt the paths in the script to your environment.

    To run the application, you can execute the script from the root of the project:
    ```bash
    ./run_mi_brain.sh
    ```
    Alternatively, you can run the executable directly after setting up the environment variables as in the script.

## Running Tests

To run the full test suite, execute the following command from the build directory:

```bash
ctest -j8
```

### Fiber Bundle Fidelity & Performance Test

A specialized test suite has been established to verify coordinate fidelity (TCK vs TRK) and measure slicing performance with the new `vtkCutter` implementation.

1.  **Build the specific test**:
    ```bash
    cd build-2025
    make DiffusionIO mitkFiberBundleFidelityTest
    ```

2.  **Run the test driver**:
    ```bash
    ./bin/mitkFiberBundleFidelityTest
    ```

**Success Criteria:**
*   `VerifyGeneratePolyDataByIds`: PASS (confirms fiber subset extraction).
*   `BenchmarkVtkCutter`: Reported speed for 100k fibers (Goal is < 33ms for 30 FPS).
*   `VerifyIOFidelity`: PASS (confirms TRK coordinate shifts are resolved).

## Packaging

To create a distributable package, run the following command from the build directory:

```bash
make package
```

## Troubleshooting

### CMake build files in wrong directory

If you find that CMake generates build files (e.g., `Makefile`, `CMakeCache.txt`) in the project's root directory instead of the `build-2025` directory, you need to clean the root directory and re-run CMake.

1.  **Clean the root directory:**
    From the project's root directory, run the following command to remove the generated files. **Use with caution.**
    ```bash
    rm -f CMakeCache.txt cmake_install.cmake CPackConfig.cmake CPackSourceConfig.cmake Makefile && rm -rf CMakeFiles
    ```

2.  **Re-run CMake:**
    Follow the build instructions from step 1.

### Qt XCB Plugin

The application requires the Qt XCB platform plugin to run on Linux. The build system will try to find it. If you have a standard Qt installation, it should be found automatically. The `CMakeLists.txt` contains logic to copy the plugin during the packaging step. If you encounter issues with packaging, you might need to adjust the `CMakeLists.txt` to match the location and name of the `libqxcb.so` file in your Qt installation.