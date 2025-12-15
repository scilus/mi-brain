
#ifndef IMEKA_PROPERTY_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED
#define IMEKA_PROPERTY_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED

#include <ctkPluginActivator.h>

class PluginActivator : public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_imeka_property_eager")
  Q_INTERFACES(ctkPluginActivator)

public:
  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);
};

#endif /* IMEKA_PROPERTY_EAGER_PLUGIN_ACTIVATOR_HPP_INCLUDED  */
