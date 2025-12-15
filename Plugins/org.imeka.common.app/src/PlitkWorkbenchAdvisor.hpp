
#ifndef PLITK_WORKBENCH_ADVISOR_HPP_INCLUDED
#define PLITK_WORKBENCH_ADVISOR_HPP_INCLUDED

#include <QMessageBox>

#include <berryQtWorkbenchAdvisor.h>

#include <mitkWorkbenchUtil.h>

#include "PlitkVersion.hpp"

template <class Activator, class WorkbenchWindowAdvisor>
class PlitkWorkbenchAdvisor : public berry::QtWorkbenchAdvisor
{
public:
  PlitkWorkbenchAdvisor(
    const QString& appTitle,
    const QString& perspectiveId)
    : m_AppTitle(appTitle)
    , m_PerspectiveId(perspectiveId)
  {}

  void Initialize(berry::IWorkbenchConfigurer::Pointer configurer) override
  {
    berry::QtWorkbenchAdvisor::Initialize(configurer);

    mitk::WorkbenchUtil::SetDepartmentLogoPreference(
      ":/Images/logo_plitk.png", Activator::GetDefault()->GetPluginContext());

    configurer->SetSaveAndRestore(true);
  }

  berry::WorkbenchWindowAdvisor* CreateWorkbenchWindowAdvisor(
    berry::IWorkbenchWindowConfigurer::Pointer configurer) override
  {
    configurer->SetTitle(m_AppTitle + " " + PLITK_VERSION);
    auto windowAdvisor = new WorkbenchWindowAdvisor(this, configurer);
    windowAdvisor->Setup();
    windowAdvisor->SetProductName(m_AppTitle);
    windowAdvisor->SetWindowIcon(":/Images/logo_imeka.png");
    return windowAdvisor;
  }

  QString GetInitialWindowPerspectiveId() override
  {
    return m_PerspectiveId;
  }

  bool PreShutdown() override
  {
    QMessageBox::StandardButton button =
      QMessageBox::question(nullptr, "Are you sure?",
      "Are you sure you want to exit?",
      QMessageBox::Yes | QMessageBox::No,
      QMessageBox::No);
    return button == QMessageBox::Yes;
  }

private:
  const QString m_AppTitle;
  const QString m_PerspectiveId;
};

#endif // PLITK_WORKBENCH_ADVISOR_HPP_INCLUDED
