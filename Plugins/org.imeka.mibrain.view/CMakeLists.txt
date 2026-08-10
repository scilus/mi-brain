project(org_imeka_mibrain_view)

mitk_create_plugin(
  EXPORT_DIRECTIVE MIBRAINVIEW_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS
  MODULE_DEPENDS MitkDataTypesExt ImekaBoundingObject ImekaFiber ImekaWidgets FiberBundle MitkMultilabel
  WARNINGS_NO_ERRORS
)
