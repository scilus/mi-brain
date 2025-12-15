
#include "LocateSelectionObjectAction.hpp"

#include <berryPlatformUI.h>
#include <berryIWorkbenchPage.h>
#include <QmitkStdMultiWidgetEditor.h>

void LocateSelectionObjectAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  const auto SONode = selectedNodes[0];
  const auto center = SONode->GetData()->GetGeometry()->GetCenter();

  auto activeEditor = berry::PlatformUI::GetWorkbench()
    ->GetActiveWorkbenchWindow()->GetActivePage()->GetActiveEditor();
  auto stdMultiWidgetEditor = activeEditor.Cast<QmitkStdMultiWidgetEditor>();
  stdMultiWidgetEditor->SetSelectedPosition(center);
}
