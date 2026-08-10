
#include "PluginActivator.hpp"

#include <usModuleInitialization.h>
US_INITIALIZE_MODULE

#include "actions/LocateSurfaceAction.hpp"
#include "actions/LocateSelectionObjectAction.hpp"
#include "actions/MakeTimedSequenceAction.hpp"
#include "actions/ReloadFromDiskAction.hpp"
#include "actions/ToggleRGBAction.hpp"

void PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(LocateSurfaceAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(LocateSelectionObjectAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(MakeTimedSequenceAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ReloadFromDiskAction, context)
  BERRY_REGISTER_EXTENSION_CLASS(ToggleRGBAction, context)
}

void PluginActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}
