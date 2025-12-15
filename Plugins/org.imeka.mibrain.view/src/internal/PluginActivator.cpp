
#include "PluginActivator.hpp"

#include "BrainAnalysisView.h"
#include "actions/colors/ColorEndPointsAction.hpp"
#include "actions/colors/ColorFromAnatomyAction.hpp"
#include "actions/colors/ColorLocalAction.hpp"
#include "actions/colors/ColorShuffleAction.hpp"
#include "actions/colors/ColorShuffleMasksAction.hpp"
#include "actions/colors/ColorUniformAction.hpp"
#include "actions/colors/LoadTextAction.hpp"
#include "actions/BinaryImageAction.hpp"
#include "actions/CreateCubeSOAction.hpp"
#include "actions/CreateSphereSOAction.hpp"
#include "actions/CreateTractGroupAction.hpp"
#include "actions/CutFibersInsideAction.hpp"
#include "actions/CutFibersOutsideAction.hpp"
#include "actions/DensityImageAction.hpp"
#include "actions/DuplicateTractGroupAction.hpp"
#include "actions/EditActiveROIsAction.hpp"
#include "actions/FlipFibersAxialAction.hpp"
#include "actions/FlipFibersSagittalAction.hpp"
#include "actions/FlipFibersCoronalAction.hpp"
#include "actions/SaveVisibleFibersDMAction.hpp"
#include "actions/SaveVisibleFibersFileAction.hpp"
#include "actions/ShuffleTractsAction.hpp"
#include "actions/SOToImageAction.hpp"
#include "actions/Toggle2DAction.hpp"
#include "actions/TogglePeaksStatusAction.hpp"

void PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(BrainAnalysisView, context)
  BERRY_REGISTER_EXTENSION_CLASS(BinaryImageAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorEndPointsAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorFromAnatomyAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorLocalAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorShuffleAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorShuffleMasksAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ColorUniformAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(LoadTextAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(CreateCubeSOAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(CreateSphereSOAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(CreateTractGroupAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(CutFibersInsideAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(CutFibersOutsideAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(DensityImageAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(DuplicateTractGroupAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(EditActiveROIsAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(FlipFibersAxialAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(FlipFibersSagittalAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(FlipFibersCoronalAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(SaveFibersToDMAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(SaveFibersToFileAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ShuffleTractsAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(SOToImageAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(Toggle2DAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(TogglePeaksStatusAction, context)
}
