set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  PluginActivator.cpp
  BrainAnalysisView.cpp
  actions/colors/ColorAbstractAction.cpp
  actions/colors/LoadTextAction.cpp
  actions/colors/LoadTextActionDialog.cpp
  actions/BinaryImageAction.cpp
  actions/CreateCubeSOAction.cpp
  actions/CreateSphereSOAction.cpp
  actions/CreateTractGroupAction.cpp
  actions/CutFibersAbstractAction.cpp
  actions/ConvertToBinaryROIAction.cpp
  actions/ConvertToMultiLabelAction.cpp
  actions/DensityImageAction.cpp
  actions/DuplicateTractGroupAction.cpp
  actions/EditActiveROIsAction.cpp
  actions/FlipFibersAbstractAction.cpp
  actions/SaveVisibleFibersAbstractAction.cpp
  actions/ShuffleTractsAction.cpp
  actions/SOToImageAction.cpp
  actions/Toggle2DAction.cpp
  actions/TogglePeaksStatusAction.cpp
  actions/utils.cpp
)

set(UI_FILES
  src/internal/actions/colors/LoadTextActionDialog.ui
  src/internal/BrainAnalysisViewControls.ui
)

set(MOC_H_FILES
  src/internal/PluginActivator.hpp
  src/internal/BrainAnalysisView.h
  src/internal/actions/colors/ColorEndPointsAction.hpp
  src/internal/actions/colors/ColorFromAnatomyAction.hpp
  src/internal/actions/colors/ColorLocalAction.hpp
  src/internal/actions/colors/ColorShuffleAction.hpp
  src/internal/actions/colors/ColorShuffleMasksAction.hpp
  src/internal/actions/colors/ColorUniformAction.hpp
  src/internal/actions/colors/LoadTextAction.hpp
  src/internal/actions/colors/LoadTextActionDialog.hpp
  src/internal/actions/BinaryImageAction.hpp
  src/internal/actions/CreateCubeSOAction.hpp
  src/internal/actions/CreateSphereSOAction.hpp
  src/internal/actions/CreateTractGroupAction.hpp
  src/internal/actions/CutFibersInsideAction.hpp
  src/internal/actions/CutFibersOutsideAction.hpp
  src/internal/actions/ConvertToBinaryROIAction.hpp
  src/internal/actions/ConvertToMultiLabelAction.hpp
  src/internal/actions/DensityImageAction.hpp
  src/internal/actions/DuplicateTractGroupAction.hpp
  src/internal/actions/EditActiveROIsAction.hpp
  src/internal/actions/FlipFibersAxialAction.hpp
  src/internal/actions/FlipFibersSagittalAction.hpp
  src/internal/actions/FlipFibersCoronalAction.hpp
  src/internal/actions/SaveVisibleFibersDMAction.hpp
  src/internal/actions/SaveVisibleFibersFileAction.hpp
  src/internal/actions/ShuffleTractsAction.hpp
  src/internal/actions/SOToImageAction.hpp
  src/internal/actions/Toggle2DAction.hpp
  src/internal/actions/TogglePeaksStatusAction.hpp
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  plugin.xml

  resources/icon.xpm
  resources/2D.png
  resources/ColorEndPoints.png
  resources/ColorFromAnatomy.png
  resources/ColorLocal.png
  resources/ColorShuffle.png
  resources/ColorUniform.png
  resources/Cube.png
  resources/CutFibersIn.png
  resources/CutFibersOut.png
  resources/Duplicate.png
  resources/GenerateBinary.png
  resources/GenerateDensity.png
  resources/GenerateImage.png
  resources/LoadColors.png
  resources/MirrorAntPost.png
  resources/MirrorInfSup.png
  resources/MirrorLeftRight.png
  resources/Pencil.png
  resources/Plus.png
  resources/Save.png
  resources/Sphere.png
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
  resources/MIBrain.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})

