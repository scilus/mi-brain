
#ifndef IMEKA_MIBRAIN_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_MIBRAIN_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <berryAbstractUICTKPlugin.h>

class PluginActivator : public berry::AbstractUICTKPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_mibrain_app")
  Q_INTERFACES(ctkPluginActivator)

public:
  PluginActivator()
    : context(nullptr)
  {
    inst = this;
  }

  static PluginActivator* GetDefault() { return inst; }
  ctkPluginContext* GetPluginContext() const { return context; }

  void start(ctkPluginContext*) override;

private:
  static PluginActivator* inst;
  ctkPluginContext* context;
};

#endif // IMEKA_MIBRAIN_APP_PLUGIN_ACTIVATOR_HPP_INCLUDED
