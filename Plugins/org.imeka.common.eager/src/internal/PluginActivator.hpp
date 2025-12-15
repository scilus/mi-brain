
#ifndef IMEKA_COMMON_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_COMMON_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <ctkPluginActivator.h>

class PluginActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_common_eager")
  Q_INTERFACES(ctkPluginActivator)

public:
  static ctkPluginContext* GetContext();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  static ctkPluginContext* m_Context;

}; // PluginActivator

#endif // IMEKA_COMMON_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED
