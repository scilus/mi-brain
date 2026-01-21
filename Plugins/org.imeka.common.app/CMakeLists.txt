project(org_imeka_common_app)

include_directories(${CMAKE_BINARY_DIR})

mitk_create_plugin(
  EXPORT_DIRECTIVE COMMONAPP_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgets ImekaGeometry
  WARNINGS_NO_ERRORS
  QRC_FILES resources/resources.qrc
)
