include(ExternalProject)

set(proj imeka-plitk-data)
set(proj_DEPENDENCIES)
set(MITK-Data_DEPENDS ${proj})

if(BUILD_TESTING)
  # MITK 2025: Test data repository not available - create empty directory
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/imeka-plitk-data-prefix/src/${proj})
  
  # Create a dummy external project that does nothing
  ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping test data download"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS ${proj_DEPENDENCIES}
  )
  
  set(PLITK_DATA_DIR ${CMAKE_BINARY_DIR}/imeka-plitk-data-prefix/src/${proj})
  configure_file(plitkTestingConfig.hpp.in ${PROJECT_BINARY_DIR}/plitkTestingConfig.hpp)
else()
  mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
endif(BUILD_TESTING)
