set(SRC_CPP_FILES
  AboutDialog.cpp
  CommonPerspective.cpp
  CommonWorkbenchWindowAdvisor.cpp
  PlitkActionBarAdvisor.cpp
  PlitkWorkbenchAdvisor.hpp
)

set(INTERNAL_CPP_FILES
  PluginActivator.cpp
  ViewPlacerAction.cpp
)

set(UI_FILES
  resources/ui/AboutDialog.ui
)

set(MOC_H_FILES
  src/AboutDialog.hpp
  src/CommonPerspective.hpp
  src/internal/PluginActivator.hpp
  src/internal/ViewPlacerAction.hpp
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  resources/icon.xpm
  plugin.xml
  resources/images/logo_plitk.png
  resources/images/logo_scil.png
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
  resources/resources.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
