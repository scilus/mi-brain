
#ifndef PLITK_CORE_APPLICATION_HPP_INCLUDED
#define PLITK_CORE_APPLICATION_HPP_INCLUDED

#include <berryIApplication.h>

class PlitkApplication : public QObject, public berry::IApplication
{
  Q_OBJECT
  Q_INTERFACES(berry::IApplication)

public:
  PlitkApplication() {}
  ~PlitkApplication() {}

  QVariant Start(berry::IApplicationContext*) override;
  void Stop() override {}
};

#endif // PLITK_CORE_APPLICATION_HPP_INCLUDED
