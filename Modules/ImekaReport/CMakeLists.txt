MITK_CREATE_MODULE( ImekaReport
  PACKAGE_DEPENDS
  DEPENDS MitkQtWidgetsExt
  WARNINGS_NO_ERRORS
)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
