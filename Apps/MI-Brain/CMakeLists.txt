
set(_app_options)
if(${MY_PROJECT_NAME}_SHOW_CONSOLE_WINDOW)
  list(APPEND _app_options SHOW_CONSOLE)
endif()

set(_plugins
  org.blueberry.core.commands
  org.blueberry.core.expressions
  org.blueberry.core.runtime
  org.blueberry.ui.qt.log
  org.blueberry.ui.qt

  org.commontk.configadmin
  org.mitk.core.services
  org.mitk.gui.common
  org.mitk.gui.qt.common
  org.mitk.gui.qt.application
  org.mitk.gui.qt.datamanager
  org.mitk.gui.qt.ext
  org.mitk.gui.qt.extapplication
  org.mitk.gui.qt.imagenavigator
  org.mitk.gui.qt.moviemaker
  org.mitk.gui.qt.properties
  org.mitk.gui.qt.segmentation

  org.imeka.common.app
  org.imeka.common.eager
  org.imeka.common.view
  org.imeka.mibrain.app
  org.imeka.mibrain.eager
  org.imeka.mibrain.view
  org.imeka.mibrain.rtt.view
  
  org.imeka.property.eager
)

mitkFunctionCreateBlueBerryApplication(
  NAME ${MY_APP_NAME}
  DESCRIPTION "MITK - ${MY_APP_NAME} Application"
  PLUGINS ${_plugins}
  ${_app_options}
)
mitk_use_modules(TARGET ${MY_APP_NAME}
  MODULES MitkAppUtil
  PACKAGES Qt6|Widgets
)

#Setting application icon for mac os x systems
set_target_properties(MI-Brain PROPERTIES MACOSX_BUNDLE_ICON_FILE "icon.icns")
if(APPLE)
  install(FILES "icons/icon.icns" DESTINATION "MI-Brain.app/Contents/Resources")
endif(APPLE)
