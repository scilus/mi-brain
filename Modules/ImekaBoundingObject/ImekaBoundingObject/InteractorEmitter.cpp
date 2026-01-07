
#include "InteractorEmitter.hpp"

#include <mitkBoundingObject.h>
#include <mitkInteractionPositionEvent.h>

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

void InteractorEmitter::InitAction(
  mitk::StateMachineAction* action,
  mitk::InteractionEvent* event)
{
  (void)action;
  (void)event;
  GetDataNode()->SetFloatProperty("opacity", 0.8);
  m_NodeIsInited = true;
  emit Selected(m_BoundingObject);
}

void InteractorEmitter::TranslateObject(
  mitk::StateMachineAction* action, mitk::InteractionEvent* event)
{
  emit HasBeenChanged(m_BoundingObject);
  Superclass::TranslateObject(action, event);
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