MITK_CREATE_MODULE( ImekaBoundingObject
  PACKAGE_DEPENDS Qt6|Core+Widgets
  DEPENDS MitkDataTypesExt MitkQtWidgets ImekaCommon
  WARNINGS_NO_ERRORS
)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
