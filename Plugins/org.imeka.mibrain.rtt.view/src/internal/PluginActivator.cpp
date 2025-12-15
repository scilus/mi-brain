
#include "PluginActivator.hpp"

#include "BrainRTTView.hpp"

void PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(BrainRTTView, context)
}
