# Testing

## Framework
- **CppUnit:** Integrated with MITK testing macros (`mitkTestingMacros.h`).

## Organization
- **Location:** `Testing` subdirectory within each module (e.g., `Modules/ImekaFiber/Testing/`).
- **Registration:** Uses `MITK_CREATE_MODULE_TESTS` in `CMakeLists.txt`.

## Patterns
- Test suites inherit from base tester classes.
- Uses `CPPUNIT_TEST_SUITE` and `MITK_TEST` macros.
- Implements `setUp()` and `tearDown()` for fixtures.
- Test data path configured via `plitkTestingConfig.hpp.in`.
