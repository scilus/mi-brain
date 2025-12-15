
#ifndef IMEKA_MIBRAIN_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_MIBRAIN_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <ctkPluginActivator.h>

class PluginActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_mibrain_eager")
  Q_INTERFACES(ctkPluginActivator)

public:
  static ctkPluginContext* GetContext();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  static ctkPluginContext* m_Context;

}; // PluginActivator

#endif // IMEKA_MIBRAIN_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED
