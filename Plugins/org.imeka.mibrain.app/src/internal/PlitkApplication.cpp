
#include "PlitkApplication.hpp"

#include <berryPlatformUI.h>

#ifdef IMEKA_MIBRAIN_LICENSE
#include "ImekaLicense/LicenseCheck.hpp"
#endif
#include "PlitkWorkbenchAdvisor.hpp"
#include "PlitkWorkbenchWindowAdvisor.hpp"
#include "PluginActivator.hpp"

QVariant PlitkApplication::Start(berry::IApplicationContext*)
{
#ifdef IMEKA_MIBRAIN_LICENSE
  if (!Imeka::License::IsApplicationLicenseValid()) { return -1; }
#endif
  typedef PlitkWorkbenchAdvisor<
    PluginActivator, PlitkWorkbenchWindowAdvisor> WA;
  const int code = berry::PlatformUI::CreateAndRunWorkbench(
    berry::PlatformUI::CreateDisplay(),
    new WA("MI-Brain", "org.imeka.plitk.mibrainperspective"));

  return code == berry::PlatformUI::RETURN_RESTART ? EXIT_RESTART : EXIT_OK;
}
