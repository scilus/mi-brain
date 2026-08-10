
#include "PlitkWorkbenchWindowAdvisor.hpp"

#include <QMainWindow>
#include <QMenuBar>

#include <berryIWorkbenchPage.h>
#include <berryIWorkbenchWindow.h>

#include "Updater.hpp"

PlitkWorkbenchWindowAdvisor::PlitkWorkbenchWindowAdvisor(
  berry::WorkbenchAdvisor* workbenchAdvisor,
  berry::IWorkbenchWindowConfigurer::Pointer configurer)
  : CommonWorkbenchWindowAdvisor(workbenchAdvisor, configurer)
{
  UseViewPlacerHotkeys();
  
  // This adds the 3D orientation marker to the 3D view.
  // We don't want it. It disappears when the display plane
  // is removed anyways and never comes back. It also appears
  // in screenshots, which is not good.
  // UseMarkerWidget();
}

void PlitkWorkbenchWindowAdvisor::PostWindowCreate()
{
  CommonWorkbenchWindowAdvisor::PostWindowCreate();

  // very bad hack...
  berry::IWorkbenchWindow::Pointer window =
    this->GetWindowConfigurer()->GetWindow();
  auto mainWindow =
    static_cast<QMainWindow*>(window->GetShell()->GetControl());
  auto menuBar = mainWindow->menuBar();

  menuBar->addMenu(new UpdaterMenu(menuBar));
  mainWindow->showMaximized();

  berry::IWorkbenchPage::Pointer page = window->GetActivePage();
  page->ShowView("org.imeka.views.brainanalysisview");
}

QList<QString> PlitkWorkbenchWindowAdvisor::ViewsToExclude()
{
  return CommonWorkbenchWindowAdvisor::ViewsToExclude()
    << "org.imeka.views.brainanalysisview"
    << "org.mitk.views.deformableclippingplane"
    << "org.mitk.views.moviemaker";
}
