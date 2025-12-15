project(org_imeka_property_eager)

mitk_create_plugin(
  EXPORT_DIRECTIVE ORG_IMEKA_PROPERTY_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgets ImekaCommon
  WARNINGS_NO_ERRORS
)
