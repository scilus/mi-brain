
#include "LocateSurfaceAction.hpp"

#include <berryPlatformUI.h>
#include <berryIWorkbenchPage.h>
#include <QmitkStdMultiWidgetEditor.h>

#include <mitkSurface.h>

#include <vtkCenterOfMass.h>
#include <vtkPolyData.h>

void LocateSurfaceAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  const auto surfaceNode = selectedNodes[0];
  const auto surface = dynamic_cast<mitk::Surface*>(surfaceNode->GetData());

  auto centerOfMass = vtkSmartPointer<vtkCenterOfMass>::New();
  centerOfMass->SetInputData(surface->GetVtkPolyData());
  centerOfMass->SetUseScalarsAsWeights(false);
  centerOfMass->Update();

  mitk::Point3D center;
  centerOfMass->GetCenter(center.Begin());

  auto activeEditor = berry::PlatformUI::GetWorkbench()
    ->GetActiveWorkbenchWindow()->GetActivePage()->GetActiveEditor();
  auto stdMultiWidgetEditor = activeEditor.Cast<QmitkStdMultiWidgetEditor>();
  stdMultiWidgetEditor->SetSelectedPosition(center);
}
