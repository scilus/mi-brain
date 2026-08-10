MITK_CREATE_MODULE( ImekaFiber
  INCLUDE_DIRS ${CMAKE_BINARY_DIR}
  PACKAGE_DEPENDS Qt6|Core
  DEPENDS MitkCore MitkTestingHelper MitkDataTypesExt MitkPersistence MitkSegmentation FiberBundle ImekaBoundingObject ImekaCommon ImekaGeometry ImekaWidgets
  WARNINGS_NO_ERRORS
)

add_definitions(-DBOOST_TIMER_ENABLE_DEPRECATED)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
