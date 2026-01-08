
#include "ViewUtils.hpp"

#include <vtkCamera.h>

#include <mitkBaseRenderer.h>
#include <mitkCameraController.h>

namespace Imeka
{

namespace View
{

mitk::BaseRenderer* GetRenderer(const ViewDirection viewDir)
{
  switch (viewDir)
  {
  case ViewDirection::Axial: return GetAxialRenderer();
  case ViewDirection::Sagittal: return GetSagittalRenderer();
  case ViewDirection::Coronal: return GetCoronalRenderer();
  case ViewDirection::Original: return Get3DRenderer();
  }
  return nullptr;
}

mitk::BaseRenderer* GetRenderer(const std::string& viewName)
{
  const auto renderWindow =
    mitk::BaseRenderer::GetRenderWindowByName(viewName);
  return mitk::BaseRenderer::GetInstance(renderWindow);
}

mitk::BaseRenderer* GetAxialRenderer()
{
  return GetRenderer("stdmulti.widget1");
}

mitk::BaseRenderer* GetSagittalRenderer()
{
  return GetRenderer("stdmulti.widget2");
}

mitk::BaseRenderer* GetCoronalRenderer()
{
  return GetRenderer("stdmulti.widget3");
}

mitk::BaseRenderer* Get3DRenderer()
{
  return GetRenderer("stdmulti.widget4");
}

std::vector<mitk::BaseRenderer*> Get2DRenderers()
{
  return{
    GetAxialRenderer(),
    GetSagittalRenderer(),
    GetCoronalRenderer()
  };
}

std::vector<vtkCamera*> Get2DCameras()
{
  return{
    GetAxialRenderer()->GetVtkRenderer()->GetActiveCamera(),
    GetSagittalRenderer()->GetVtkRenderer()->GetActiveCamera(),
    GetCoronalRenderer()->GetVtkRenderer()->GetActiveCamera()
  };
}

mitk::Point3D GetCrosshairPosition()
{
  mitk::Point3D position;
  const auto cameras = Get2DCameras();
  position[0] = cameras[1]->GetFocalPoint()[0];
  position[1] = cameras[2]->GetFocalPoint()[1];
  position[2] = cameras[0]->GetFocalPoint()[2];
  return position;
}

void SetCrosshairPosition(const mitk::Point3D& position)
{
  for (auto renderer : Get2DRenderers())
  {
    mitk::SliceNavigationController* nc =
      renderer->GetSliceNavigationController();
    nc->SelectSliceByPoint(position);

    mitk::Point2D pointOnPlane;
    renderer->GetCurrentWorldPlaneGeometry()->Map(position, pointOnPlane);
    renderer->GetCameraController()->MoveCameraToPoint(pointOnPlane);
  }

}

} // namespace View

} // namespace Imeka
