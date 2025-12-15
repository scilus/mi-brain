project(org_imeka_common_view)

mitk_create_plugin(
  EXPORT_DIRECTIVE COMMONVIEW_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkCore MitkDataTypesExt ImekaBoundingObject ImekaCommon #ImekaFiber
  # MitkCore doesn't actually do any useful import, but we need at least one plugin who import MitkCore, otherwise ImekaIO won't be autoloaded.
  WARNINGS_NO_ERRORS
)
