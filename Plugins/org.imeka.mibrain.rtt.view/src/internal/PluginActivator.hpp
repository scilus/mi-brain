
#ifndef IMEKA_MIBRAIN_RTT_VIEW_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_MIBRAIN_RTT_VIEW_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <ctkPluginActivator.h>

class PluginActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_mibrain_rtt_view")
  Q_INTERFACES(ctkPluginActivator)

public:
  void start(ctkPluginContext* context);
  void stop(ctkPluginContext*) {}

}; // PluginActivator

#endif // IMEKA_MIBRAIN_RTT_VIEW_PLUGIN_ACTIVATOR_HPP_INCLUDED
