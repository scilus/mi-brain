
#ifndef PLITK_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED
#define PLITK_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED

#include "CommonWorkbenchWindowAdvisor.hpp"

class PlitkWorkbenchWindowAdvisor : public CommonWorkbenchWindowAdvisor
{
public:
  PlitkWorkbenchWindowAdvisor(
    berry::WorkbenchAdvisor*,
    berry::IWorkbenchWindowConfigurer::Pointer);

protected:
  void PostWindowCreate() override;

  virtual QList<QString> ViewsToExclude() override;
};

#endif // PLITK_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED
