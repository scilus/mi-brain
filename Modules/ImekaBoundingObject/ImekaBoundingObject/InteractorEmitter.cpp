
#include "InteractorEmitter.hpp"

#include <mitkBoundingObject.h>
#include <mitkInteractionPositionEvent.h>
#include <vtkCamera.h>

namespace Imeka
{

namespace BoundingObject
{

InteractorEmitter::InteractorEmitter(
  mitk::DataNode* dataNode)
  : AffineImageCropperInteractor()
  , m_BoundingObject(dynamic_cast<mitk::BoundingObject*>(dataNode->GetData()))
  , m_NodeIsSelected(false)
  , m_NodeIsInited(false)
{}

// Override mitk::AffineImageCropperInteractor::CheckOverObject() because it
// is using PickWorldPoint(), which has 3 methods to pick a point but all of
// them are unreliable!
// With the current method here, we use PickObject() which concentrates its
// effort on finding a node. It seems pretty reliable.
bool InteractorEmitter::CheckOverObject(const mitk::InteractionEvent* event)
{
  const auto positionEvent =
    dynamic_cast<const mitk::InteractionPositionEvent*>(event);
  if (!positionEvent) { return false; }

  // The standard CheckOverObject handles the 2D case perfectly
  const auto renderer = positionEvent->GetSender();
  if (renderer->GetMapperID() == mitk::BaseRenderer::Standard2D)
  {
    return Superclass::CheckOverObject(event);
  }

  mitk::Point3D currentPickedPoint;
  const auto pickedNode = renderer->PickObject(
    positionEvent->GetPointerPositionOnScreen(), currentPickedPoint);
  return pickedNode == GetDataNode();
}

void InteractorEmitter::SelectObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  Superclass::SelectObject(action, event);

  mitk::DataNode::Pointer dn = this->GetDataNode();

  if (dn.IsNull())
    return;

  m_SelectedNode = dn;

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  if (m_NodeIsSelected)
  {
    return;
  }

  m_NodeIsSelected = true;
  float originalOpacity = 0.0;
  GetDataNode()->GetFloatProperty("opacity", originalOpacity);
  GetDataNode()->SetFloatProperty("originalOpacity", originalOpacity);
  GetDataNode()->SetFloatProperty("opacity", 0.6);
}

void InteractorEmitter::DeselectObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  Superclass::DeselectObject(action, event);

  mitk::DataNode::Pointer dn = this->GetDataNode();

  if (dn.IsNull())
    return;

  m_SelectedNode = dn;

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  m_NodeIsSelected = false;
  float originalOpacity = 0.4;
  GetDataNode()->GetFloatProperty("originalOpacity", originalOpacity);
  GetDataNode()->SetFloatProperty("opacity", originalOpacity);
  if (m_NodeIsInited)
  {
    m_NodeIsInited = false;
    emit Unselected(m_BoundingObject);
  }
}

// behavior is odd, scales from the cursor position, not the object origin.
// So we just intercept and do nothing. This hack is probably not great, but it works.
void InteractorEmitter::ScaleRadius(mitk::StateMachineAction *action, mitk::InteractionEvent *event)
{
  (void) action; (void) event;
  return;
  // GetDataNode()->SetFloatProperty("opacity", 0.1);
  // m_NodeIsInited = true;
  // emit Selected(m_BoundingObject);
  // // Superclass::ScaleRadius(action, event);
}

static bool RayPlaneIntersection(const mitk::Point3D& rayOrigin,
                          const mitk::Vector3D& rayDirection,
                          const mitk::Point3D& planePoint,
                          const mitk::Vector3D& planeNormal,
                          mitk::Point3D& intersection)
{
  const double denom = planeNormal[0] * rayDirection[0] +
                       planeNormal[1] * rayDirection[1] +
                       planeNormal[2] * rayDirection[2];
  if (std::abs(denom) < 1e-8)
  {
    return false;
  }

  mitk::Vector3D diff;
  diff[0] = planePoint[0] - rayOrigin[0];
  diff[1] = planePoint[1] - rayOrigin[1];
  diff[2] = planePoint[2] - rayOrigin[2];

  const double t = (planeNormal[0] * diff[0] +
                    planeNormal[1] * diff[1] +
                    planeNormal[2] * diff[2]) / denom;

  intersection[0] = rayOrigin[0] + rayDirection[0] * t;
  intersection[1] = rayOrigin[1] + rayDirection[1] * t;
  intersection[2] = rayOrigin[2] + rayDirection[2] * t;

  return true;
}

void InteractorEmitter::InitTranslate(
  mitk::StateMachineAction* action,
  mitk::InteractionEvent* event)
{
  GetDataNode()->SetFloatProperty("opacity", 0.8);
  m_NodeIsInited = true;
  emit Selected(m_BoundingObject);
  Superclass::InitTranslate(action, event);

  auto *positionEvent = dynamic_cast<mitk::InteractionPositionEvent *>(event);
  if (!positionEvent || !m_SelectedNode) {
    MITK_WARN << "position event or selected node nullptr \n";
    return;
  }

  auto renderer = positionEvent->GetSender();
  vtkCamera* camera = renderer->GetVtkRenderer()->GetActiveCamera();

  double vpn[3];
  camera->GetViewPlaneNormal(vpn);

  mitk::Point3D origin = positionEvent->GetPositionInWorld();

  m_InteractionPlaneNormal[0] = vpn[0];
  m_InteractionPlaneNormal[1] = vpn[1];
  m_InteractionPlaneNormal[2] = vpn[2];
  m_InteractionPlaneNormal.Normalize();

  m_InteractionPlanePoint = origin;

  double cameraPositionArray[3];
  camera->GetPosition(cameraPositionArray);
  mitk::Point3D cameraPosition;
  cameraPosition[0] = cameraPositionArray[0];
  cameraPosition[1] = cameraPositionArray[1];
  cameraPosition[2] = cameraPositionArray[2];

  mitk::Vector3D rayDirection;
  rayDirection[0] = origin[0] - cameraPosition[0];
  rayDirection[1] = origin[1] - cameraPosition[1];
  rayDirection[2] = origin[2] - cameraPosition[2];
  rayDirection.Normalize();

  if (!RayPlaneIntersection(cameraPosition,
                            rayDirection,
                            m_InteractionPlanePoint,
                            m_InteractionPlaneNormal,
                            m_InitialPlaneIntersection))
  {
    m_InitialPlaneIntersection = origin;
  }

  m_InitialOrigin = m_SelectedNode->GetData()->GetGeometry()->GetOrigin();
}

void InteractorEmitter::InitRotate(
  mitk::StateMachineAction* action,
  mitk::InteractionEvent* event)
{
  GetDataNode()->SetFloatProperty("opacity", 0.8);
  m_NodeIsInited = true;
  emit Selected(m_BoundingObject);
  Superclass::InitRotate(action, event);
}

void InteractorEmitter::InitDeformation(
  mitk::StateMachineAction* action,
  mitk::InteractionEvent* event)
{
  GetDataNode()->SetFloatProperty("opacity", 0.8);
  m_NodeIsInited = true;
  emit Selected(m_BoundingObject);
  Superclass::InitDeformation(action, event);
}

void InteractorEmitter::TranslateObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  (void)action;
  emit HasBeenChanged(m_BoundingObject);
  // Superclass::TranslateObject(action, event);

  auto *positionEvent = dynamic_cast<mitk::InteractionPositionEvent *>(event);
  if (!positionEvent || !m_SelectedNode) {
    MITK_WARN << "position event or selected node nullptr \n";
    return;}

  auto renderer = positionEvent->GetSender();
  vtkCamera* camera = renderer->GetVtkRenderer()->GetActiveCamera();

  double cameraPositionArray[3];
  camera->GetPosition(cameraPositionArray);
  mitk::Point3D cameraPosition;
  cameraPosition[0] = cameraPositionArray[0];
  cameraPosition[1] = cameraPositionArray[1];
  cameraPosition[2] = cameraPositionArray[2];

  mitk::Point3D currentWorld = positionEvent->GetPositionInWorld();
  mitk::Vector3D rayDirection;
  rayDirection[0] = currentWorld[0] - cameraPosition[0];
  rayDirection[1] = currentWorld[1] - cameraPosition[1];
  rayDirection[2] = currentWorld[2] - cameraPosition[2];
  rayDirection.Normalize();

  mitk::Point3D currentIntersection;
  if (!RayPlaneIntersection(cameraPosition,
                            rayDirection,
                            m_InteractionPlanePoint,
                            m_InteractionPlaneNormal,
                            currentIntersection))
  {
    return;
  }

  mitk::Vector3D interactionMove;
  interactionMove[0] = currentIntersection[0] - m_InitialPlaneIntersection[0];
  interactionMove[1] = currentIntersection[1] - m_InitialPlaneIntersection[1];
  interactionMove[2] = currentIntersection[2] - m_InitialPlaneIntersection[2];

  mitk::BaseGeometry::Pointer geometry = m_SelectedNode->GetData()->GetGeometry();
  geometry->SetOrigin(m_InitialOrigin);
  geometry->Translate(interactionMove);

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void InteractorEmitter::RotateObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  emit HasBeenChanged(m_BoundingObject);
  Superclass::RotateObject(action, event);
}

void InteractorEmitter::DeformObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  emit HasBeenChanged(m_BoundingObject);
  Superclass::DeformObject(action, event);
}

} // namespace BoundingObject

} // namespace Imeka