project(org_imeka_common_app)

include_directories(${CMAKE_BINARY_DIR})

qt6_add_resources(COMMONAPP_RESOURCES
    resources/resources.qrc
    org_imeka_common_app_cached.qrc
)

mitk_create_plugin(
  EXPORT_DIRECTIVE COMMONAPP_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgets ImekaGeometry
  WARNINGS_NO_ERRORS
  RESOURCES ${COMMONAPP_RESOURCES}
)
