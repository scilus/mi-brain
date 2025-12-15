
#include "DataManager.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <mitkDataNode.h>
#include <mitkNodePredicateData.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include "ImekaCommon/Predicate.hpp"

namespace Imeka
{

const char* DataManager::UUIDPropertyName = "UUID";

// Give a UUID to all objects that doesn't already have one
void DataManager::GiveUUID(mitk::DataNode* node) const
{
  std::string uuid_str = "";
  node->GetStringProperty(UUIDPropertyName, uuid_str);
  if (uuid_str == "")
  {
    const auto uuid = boost::uuids::random_generator()();
    uuid_str = boost::uuids::to_string(uuid);
    node->SetStringProperty(UUIDPropertyName, uuid_str.c_str());
  }
}

Nodes DataManager::AllNodesRelatedByUUID(
  const mitk::DataNode* node,
  const char* propertyName,
  const std::string& linkPropertyName) const
{
  std::string uuid_str = "";
  node->GetStringProperty(propertyName, uuid_str);
  return GetAll(Imeka::Predicate::Property(
    linkPropertyName.c_str(), uuid_str));
}

mitk::DataNode* DataManager::OnlyLinkedByUUID(
  const mitk::DataNode* node,
  const char* propertyName,
  const std::string& linkPropertyName) const
{
  const auto allRelated =
    AllNodesRelatedByUUID(node, propertyName, linkPropertyName);
  if (allRelated.size() > 1)
  {
    throw "More than one node is linked to this UUID.";
  }

  return allRelated[0];
}

void DataManager::CopyUUID(
  const mitk::DataNode* fromNode,
  mitk::DataNode* toNode,
  const char* destinationPropertyName) const
{
  std::string uuid_str = "";
  fromNode->GetStringProperty(UUIDPropertyName, uuid_str);
  toNode->SetStringProperty(destinationPropertyName, uuid_str.c_str());
}

std::string DataManager::FirstFreeName(
  const QString& pattern,
  const unsigned int startingIdx,
  const unsigned int nbPadding) const
{
  for (unsigned int idx = startingIdx; /**/; ++idx)
  {
    const std::string name =
      pattern.arg(idx, nbPadding, 10, QChar('0')).toStdString();
    if (!m_DataStorage->GetNamedNode(name))
    {
      return name;
    }
  }
}

DataManager::DataManager(mitk::DataStorage* ds)
  : m_DataStorage(ds)
{}

unsigned int DataManager::NumberOfVisibleObjects() const
{
  auto isVisible = mitk::NodePredicateNot::New(
    mitk::NodePredicateProperty::New(
    "includeInBoundingBox", mitk::BoolProperty::New(false)));
  const auto nodes = m_DataStorage->GetSubset(isVisible);
  return nodes->Size() - 1; // -1 because of "widgets" node.
}

bool DataManager::AtLeastOneVisible(const std::string& dataType) const
{
  return Imeka::Predicate::AtLeastOne(dataType, m_DataStorage, true, false);
}

void DataManager::SetAnat(mitk::DataNode* node) const
{
  const std::string name = "Anat";
  for (auto node : GetAll(Imeka::Predicate::Property(name.c_str(), true)))
  {
    node->GetPropertyList()->DeleteProperty(name);
  }
  if (node)
  {
    node->SetBoolProperty("Anat", true);
  }
}

mitk::DataNode* DataManager::GetAnat() const
{
  const auto nodes = GetAll(Imeka::Predicate::Property("Anat", true));
  assert(nodes.size() <= 1);

  if (nodes.empty()) { return nullptr; }
  return nodes[0];
}

Nodes DataManager::GetAll(
  const std::string& dataType,
  const mitk::DataNode* parent) const
{
  itk::VectorContainer<
    unsigned int, mitk::DataNode::Pointer>::ConstPointer nodes;
  if (dataType == "")
  {
    if (!parent)
    {
      nodes = m_DataStorage->GetAll();
    }
    else
    {
      nodes = m_DataStorage->GetDerivations(parent, nullptr, false);
    }
  }
  else
  {
    mitk::NodePredicateBase::Pointer predicate =
      Imeka::Predicate::GetPredicateFor(dataType);
    if (!parent)
    {
      nodes = m_DataStorage->GetSubset(predicate);
    }
    else
    {
      nodes = m_DataStorage->GetDerivations(parent, predicate, false);
    }
  }

  const auto res = nodes->CastToSTLConstContainer();
  return Nodes(std::begin(res), std::end(res));
}

Nodes DataManager::GetAll(
  mitk::NodePredicateBase* pred,
  const mitk::DataNode* parent) const
{
  itk::VectorContainer<unsigned int,
                       mitk::DataNode::Pointer>::ConstPointer res;
  if (parent)
  {
    res = m_DataStorage->GetDerivations(parent, pred);
  }
  else
  {
    res = m_DataStorage->GetSubset(pred);
  }
  const auto v = res->CastToSTLConstContainer();
  return Nodes(std::begin(v), std::end(v));
}

mitk::DataNode* DataManager::GetNodeContainingThis(
  mitk::BaseData* baseData) const
{
  // baseData can't be const because of NodePredicateData
  const auto v = GetAll(mitk::NodePredicateData::New(baseData));
  if (v.empty()) { return nullptr; }
  return v[0];
}

Nodes DataManager::GetNodesInTree(
  const mitk::DataNode* parentNode,
  const bool onlyDirectChildren,
  const bool includeParentNode) const
{
  const auto childrenNode =
    m_DataStorage->GetDerivations(parentNode, nullptr, onlyDirectChildren)
      ->CastToSTLConstContainer();

  auto nodes = Nodes(std::begin(childrenNode), std::end(childrenNode));
  if (includeParentNode)
  {
    nodes.push_back(const_cast<mitk::DataNode*>(parentNode));
  }
  return nodes;
}

void DataManager::ShowAllParents(const mitk::DataNode* nodeToShow)
{
  const auto parent = GetParent(nodeToShow);
  if (parent)
  {
    parent->SetVisibility(true);
    ShowAllParents(parent);
  }
}

void DataManager::ShowAllChildren(const mitk::DataNode* nodeToShow)
{
  const auto children = ChildrenOf(nodeToShow);
  for (auto node : children)
  {
    node->SetBoolProperty("visible", true);
  };
}

void DataManager::HideAllChildren(const mitk::DataNode* nodeToHide)
{
  for (auto node : ChildrenOf(nodeToHide))
  {
    node->SetBoolProperty("visible", false);
  }
}

mitk::DataNode* DataManager::GetParent(const mitk::DataNode* node)
{
  const auto parents = m_DataStorage->GetSources(node);
  if (parents->empty()) { return nullptr; }
  return parents->front();
}

void DataManager::ChangeParent(
  mitk::DataNode* node,
  mitk::DataNode* newParent)
{
  // Don't do anything if the node already has the right parent
  if (GetParent(node) == newParent) { return; }

  // No need to delete interactor or call InitializeViewsByBoundingObjects
  // as it's the same node at the same position... same everything.
  // We simply want the node to be displayed under another node.
  mitk::DataNode::Pointer safe = node;
  m_DataStorage->Remove(node);
  m_DataStorage->Add(node, newParent);
}

void DataManager::AddNode(mitk::DataNode* node, mitk::DataNode* parent)
{
  m_DataStorage->Add(node, parent);
}

void DataManager::RemoveNode(const mitk::DataNode* node)
{
  RemoveNodeFromDataStorage(m_DataStorage, node);
}

void DataManager::RemoveAll(
  const char* objectType, const mitk::DataNode* parent)
{
  const auto nodes = GetAll(objectType, parent);
  for (auto& node : nodes)
  {
    RemoveNode(node);
  }
}

void DataManager::RemoveAllChildren(const mitk::DataNode* parent)
{
  RemoveAll("", parent);
}

void RemoveNodeFromDataStorage(
  mitk::DataStorage* storage,
  const mitk::DataNode* node)
{
  if (storage && node && storage->Exists(node))
  {
    storage->Remove(node);
  }
}

} // namespace Imeka
