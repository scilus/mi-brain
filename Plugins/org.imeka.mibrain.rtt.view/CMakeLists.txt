project(org_imeka_mibrain_rtt_view)

mitk_create_plugin(
  EXPORT_DIRECTIVE MIBRAINRTTVIEW_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS
  MODULE_DEPENDS ImekaCommon ImekaFiber
  WARNINGS_NO_ERRORS
)
