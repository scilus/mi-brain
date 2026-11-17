
#ifndef IMEKA_BOUNDING_OBJECT_NODE_UTIL_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_NODE_UTIL_HPP_INCLUDED

#include "InteractorEmitter.hpp"

#include "ImekaBoundingObjectExports.h"

namespace mitk { class BoundingObject; }

namespace Imeka
{

class DataManager;

namespace BoundingObject
{

static const char* const INTERNAL_BOUNDING_OBJECT_NAME =
  "Imeka.BoundingObject.BoundingObject";

mitk::DataNode::Pointer IMEKABOUNDINGOBJECT_EXPORT
GetBoundingObjectNode(
  const mitk::DataStorage* storage,
  const mitk::DataNode* parentNode,
  const unsigned int index = 0);

bool IMEKABOUNDINGOBJECT_EXPORT
AddBoundingObjectNodeToParent(
  mitk::DataStorage* storage,
  mitk::DataNode* parentNode,
  mitk::DataNode* boundingObjectNode,
  mitk::BaseGeometry* geometry = nullptr);

mitk::DataNode::Pointer IMEKABOUNDINGOBJECT_EXPORT
GetNewBoundingObjectNode(
  mitk::BoundingObject* boundingObject,
  const bool isHelperObject = false);

void IMEKABOUNDINGOBJECT_EXPORT
AddInteractorToAllBoundingObjectNodes(Imeka::DataManager* DM);

InteractorEmitter::Pointer IMEKABOUNDINGOBJECT_EXPORT
AddInteractor(mitk::DataNode* node);

} // namespace BoundingObject

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_NODE_UTIL_HPP_INCLUDED

