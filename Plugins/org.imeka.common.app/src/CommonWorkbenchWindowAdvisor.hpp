
#ifndef IMEKA_APP_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED
#define IMEKA_APP_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED

#include <berryIPartListener.h>
#include <berryIWorkbenchWindow.h>

#include <vtkAnnotatedCubeActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSmartPointer.h>

#include <QmitkExtWorkbenchWindowAdvisor.h>

#include "org_imeka_common_app_Export.h"

class QMainWindow;
class QMenuBar;
class QToolBar;

class COMMONAPP_EXPORT CommonWorkbenchWindowAdvisor
  : public QmitkExtWorkbenchWindowAdvisor
{
public:
  CommonWorkbenchWindowAdvisor(
    berry::WorkbenchAdvisor*,
    berry::IWorkbenchWindowConfigurer::Pointer);

  void UseMarkerWidget() { m_UseMarkerWidget = true; }
  void UseViewPlacerHotkeys() { m_SetViewPlacerHotkeys = true; }

  void PostWindowCreate() override;
  void Setup();

protected:
  virtual QList<QString> PerspectivesToExclude();
  virtual QList<QString> ViewsToExclude();

  bool m_UseMarkerWidget;
  bool m_SetViewPlacerHotkeys;
  QMenu* m_HelpMenu;

private:
  vtkSmartPointer<vtkAnnotatedCubeActor> m_AnnotatedCube;
  vtkSmartPointer<vtkOrientationMarkerWidget> m_MarkerWidget;

  QScopedPointer<berry::IPartListener> m_Listener;
};

#endif // IMEKA_APP_WORKBENCH_WINDOW_ADVISOR_HPP_INCLUDED
