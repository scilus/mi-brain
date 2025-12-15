#ifndef IMEKA_COMMON_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_COMMON_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <ctkPluginActivator.h>

class PluginActivator : public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_common_app")
  Q_INTERFACES(ctkPluginActivator)

public:
  void start(ctkPluginContext* context) override;
  void stop(ctkPluginContext* context) override;
}; // PluginActivator

#endif // IMEKA_COMMON_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED
