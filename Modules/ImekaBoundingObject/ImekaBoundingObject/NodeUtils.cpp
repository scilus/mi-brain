
#include "NodeUtils.hpp"

#include <mitkBoundingObject.h>
#include <mitkDataNode.h>
#include <mitkDataStorage.h>
#include <mitkNodePredicateOr.h>

#include <usModuleRegistry.h>

#include "ImekaCommon/DataManager.hpp"
#include "ImekaCommon/Predicate.hpp"

namespace Imeka
{

namespace BoundingObject
{

mitk::DataNode::Pointer ImekaBoundingObject_EXPORT
GetBoundingObjectNode(
  const mitk::DataStorage* storage,
  const mitk::DataNode* parentNode,
  const unsigned int index)
{
  mitk::DataNode::Pointer boundingObjectNode  = nullptr;
  if (parentNode && storage)
  {
    const auto isBO = Imeka::Predicate::IsBoundingObjectPredicate();
    const auto sceneNodes = storage->GetDerivations(parentNode, isBO, true);
    if (index < sceneNodes->size())
    {
      boundingObjectNode = sceneNodes->ElementAt(index);
    }
  }
  return boundingObjectNode;
}

bool ImekaBoundingObject_EXPORT
AddBoundingObjectNodeToParent(
  mitk::DataStorage* storage,
  mitk::DataNode* parentNode,
  mitk::DataNode* boNode,
  mitk::BaseGeometry* geometry)
{
  if (!(storage && parentNode && boNode))
  {
    MITK_WARN << "Unable to add BoundingObject node to parent\n";
    return false;
  }

  if (storage->Exists(boNode))
  {
    MITK_WARN << "Invalid image or BoundingObject already exists in storage\n";
    return false;
  }

  storage->Add(boNode, parentNode);

  if (geometry)
  {
    auto bo = dynamic_cast<mitk::BoundingObject*>(boNode->GetData());
    if (bo)
    {
      bo->FitGeometry(geometry);
    }
    else
    {
      MITK_WARN << "BoundingObject FitGeometry failed\n";
      return false;
    }
  }

  boNode->SetVisibility(true);

  return true;
}

mitk::DataNode::Pointer ImekaBoundingObject_EXPORT
GetNewBoundingObjectNode(
  mitk::BoundingObject* boundingObject,
  const bool isHelperObject)
{
  mitk::DataNode::Pointer boundingObjectNode = nullptr;
  if (boundingObject)
  {
    boundingObjectNode = mitk::DataNode::New();
    boundingObjectNode->SetData(boundingObject);
    boundingObjectNode->SetName(INTERNAL_BOUNDING_OBJECT_NAME);
    boundingObjectNode->SetColor(1.0, 1.0, 0.0);
    boundingObjectNode->SetOpacity(0.4);
    boundingObjectNode->SetIntProperty("layer", 99);
    boundingObjectNode->SetBoolProperty("helper object", isHelperObject);
  }

  return boundingObjectNode;
}

void ImekaBoundingObject_EXPORT
AddInteractorToAllBoundingObjectNodes(Imeka::DataManager* DM)
{
  if (!DM)
  {
    MITK_WARN << __FILE__ << ":" << __LINE__ << " - Invalid storage\n";
    return;
  }

  const auto isBO = Imeka::Predicate::IsBoundingObjectPredicate();
  for (auto boNode : DM->GetAll(isBO))
  {
    AddInteractor(boNode);
  }
}

InteractorEmitter::Pointer ImekaBoundingObject_EXPORT
AddInteractor(mitk::DataNode* node)
{
  if (node && !node->GetDataInteractor())
  {
    auto interactor = InteractorEmitter::New(node);
    interactor->LoadStateMachine(
      "ClippingPlaneInteraction3D.xml",
      us::ModuleRegistry::GetModule("MitkDataTypesExt"));
    interactor->SetEventConfig(
      "CropperDeformationConfig.xml",
      us::ModuleRegistry::GetModule("MitkDataTypesExt"));
    interactor->SetDataNode(node);
    return interactor;
  }

  return nullptr;
}

} // namespace BoundingObject

} // namespace Imeka
