
#include "PlitkApplication.hpp"

#include <berryPlatformUI.h>

#include "PlitkWorkbenchAdvisor.hpp"
#include "PlitkWorkbenchWindowAdvisor.hpp"
#include "PluginActivator.hpp"

QVariant PlitkApplication::Start(berry::IApplicationContext*)
{
  typedef PlitkWorkbenchAdvisor<
    PluginActivator, PlitkWorkbenchWindowAdvisor> WA;
  const int code = berry::PlatformUI::CreateAndRunWorkbench(
    berry::PlatformUI::CreateDisplay(),
    new WA("MI-Brain", "org.imeka.plitk.mibrainperspective"));

  return code == berry::PlatformUI::RETURN_RESTART ? EXIT_RESTART : EXIT_OK;
}
