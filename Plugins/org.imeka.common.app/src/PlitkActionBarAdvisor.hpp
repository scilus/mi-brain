
#ifndef IMEKA_APP_ACTION_BAR_ADVISOR_HPP_INCLUDED
#define IMEKA_APP_ACTION_BAR_ADVISOR_HPP_INCLUDED

#include <berryActionBarAdvisor.h>

#include "org_imeka_common_app_Export.h"

class COMMONAPP_EXPORT PlitkActionBarAdvisor
  : public berry::ActionBarAdvisor
{
public:
  explicit PlitkActionBarAdvisor(
    const berry::SmartPointer<berry::IActionBarConfigurer>& configurer);

protected:

  void FillMenuBar(berry::IMenuManager* menuBar) override;
};

#endif // IMEKA_APP_ACTION_BAR_ADVISOR_HPP_INCLUDED
