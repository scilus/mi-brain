
#include "PluginActivator.hpp"

#include "AboutHandler.hpp"
#include "CommonPerspective.hpp"

void PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(AboutHandler, context)
  BERRY_REGISTER_EXTENSION_CLASS(CommonPerspective, context);
}

void PluginActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}