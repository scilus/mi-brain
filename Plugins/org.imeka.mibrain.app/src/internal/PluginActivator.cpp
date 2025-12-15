
#include "PluginActivator.hpp"

#include <berryMacros.h>

#include "MIBrainPerspective.hpp"
#include "PlitkApplication.hpp"

PluginActivator* PluginActivator::inst = nullptr;

void PluginActivator::start(ctkPluginContext* context)
{
  berry::AbstractUICTKPlugin::start(context);

  this->context = context;

  BERRY_REGISTER_EXTENSION_CLASS(PlitkApplication, context);
  BERRY_REGISTER_EXTENSION_CLASS(MIBrainPerspective, context);
}
