set(SRC_CPP_FILES
  CommonView.cpp
  WidgetProperties.cpp
)

set(INTERNAL_CPP_FILES
  actions/LocateSelectionObjectAction.cpp
  actions/LocateSurfaceAction.cpp
  actions/MakeTimedSequenceAction.cpp
  actions/MakeTimedSequenceDialog.cpp
  actions/ReloadFromDiskAction.cpp
  actions/ToggleRGBAction.cpp
  PluginActivator.cpp
)

set(UI_FILES
  src/internal/actions/MakeTimedSequenceDialog.ui
)

set(MOC_H_FILES
  src/CommonView.hpp
  src/WidgetProperties.hpp
  src/internal/actions/LocateSelectionObjectAction.hpp
  src/internal/actions/LocateSurfaceAction.hpp
  src/internal/actions/MakeTimedSequenceAction.hpp
  src/internal/actions/MakeTimedSequenceDialog.hpp
  src/internal/actions/ReloadFromDiskAction.hpp
  src/internal/actions/ToggleRGBAction.hpp
  src/internal/PluginActivator.hpp
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  plugin.xml
  
  resources/Locate.png
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
  resources/Common.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})

