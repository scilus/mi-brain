# MI-Brain Build Instructions

These instructions describe how to build the MI-Brain project.

## Requirements

*   **CMake**: Version 3.18 or higher.
*   **Qt**: Version 6.5.x is recommended in the `README.rst`, but version 6.10.1 was found and used for these instructions. The build system will look for it in `/home/local/USHERBROOKE/rhef1902/Libraries/Qt/`.
*   **MITK**: Version 2025.10. The build is configured to use the MITK build located at `/home/local/USHERBROOKE/rhef1902/Libraries/MITK-2025.12`.

## Build Steps

1.  **Create and navigate to the build directory:**
    ```bash
    mkdir -p build-2025
    cd build-2025
    ```

2.  **Configure the project with CMake:**
    Run the following command from within the `build-2025` directory. This command points to the MITK and Qt6 dependency locations.

    ```bash
    cmake -DMITK_DIR=/home/local/USHERBROOKE/rhef1902/Libraries/MITK-2025.12 -DQt6_DIR=/home/local/USHERBROOKE/rhef1902/Libraries/Qt/6.10.1/gcc_64/lib/cmake/Qt6 ..
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

To run the test suite, execute the following command from the build directory:

```bash
ctest -j8
```

## Packaging

To create a distributable package, run the following command from the build directory:

```bash
make package
```
