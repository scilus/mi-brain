include(ExternalProject)

set(proj imeka-plitk-data)
set(proj_DEPENDENCIES)
set(MITK-Data_DEPENDS ${proj})

if(BUILD_TESTING)
  # ExternalProject_Add(${proj}
  #   GIT_REPOSITORY https://TODO_URL/data/plitkdata.git
  #   UPDATE_COMMAND ""
  #   CONFIGURE_COMMAND ""
  #   BUILD_COMMAND ""
  #   INSTALL_COMMAND ""
  #   TLS_VERIFY 0
  #   DEPENDS ${proj_DEPENDENCIES}
  # )
  set(PLITK_DATA_DIR ${PROJECT_SOURCE_DIR}/testing_data)
  configure_file(plitkTestingConfig.hpp.in ${PROJECT_BINARY_DIR}/plitkTestingConfig.hpp)
else()
  mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
endif(BUILD_TESTING)
