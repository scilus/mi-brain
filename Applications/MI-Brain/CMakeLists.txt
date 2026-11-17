project(MI-Brain)

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
  NAME MI-Brain
  DESCRIPTION "MI-Brainh"
  ID "org.imeka.mibrain"
  COPYRIGHT "Copyright (c) Imeka. All rights reserved."
  PLUGINS ${_plugins}
)

if(TARGET ${CMAKE_PROJECT_NAME}-autoload)
  add_dependencies(MI-Brain ${CMAKE_PROJECT_NAME}-autoload)
endif()
