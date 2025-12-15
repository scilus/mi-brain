#ifndef DATA_MANAGER_HPP_INCLUDED
#define DATA_MANAGER_HPP_INCLUDED

#include <QString>

#include "types.hpp"

#include "ImekaCommonExports.h"

namespace mitk
{
  class BaseData;
  class DataNode;
  class DataStorage;
  class NodePredicateBase;
}

namespace Imeka
{

class ImekaCommon_EXPORT DataManager
{
public:
  static const char* UUIDPropertyName;

  DataManager(mitk::DataStorage*);

  void GiveUUID(mitk::DataNode*) const;
  mitk::DataNode* OnlyLinkedByUUID(
    const mitk::DataNode*, const char*, const std::string&) const;
  Nodes AllNodesRelatedByUUID(
    const mitk::DataNode*, const char*, const std::string&) const;
  void CopyUUID(const mitk::DataNode*, mitk::DataNode*, const char*) const;

  std::string FirstFreeName(
    const QString&, const unsigned int, const unsigned int) const;
  unsigned int NumberOfVisibleObjects() const;
  mitk::DataStorage* GetDataStorage() { return m_DataStorage; }

  bool AtLeastOneVisible(const std::string&) const;

  Nodes GetAllNodes(const mitk::DataNode* parent = nullptr) const
  {
    return GetAll("", parent);
  }

  mitk::DataNode* GetNodeContainingThis(mitk::BaseData*) const;

  void SetAnat(mitk::DataNode*) const;
  mitk::DataNode* GetAnat() const;

  mitk::DataNode* GetFirst(
    mitk::NodePredicateBase* pred,
    const mitk::DataNode* parent = nullptr) const
  {
    return GetAll(pred, parent)[0];
  }
  Nodes GetAll(const std::string&, const mitk::DataNode* = nullptr) const;
  Nodes GetAll(
    mitk::NodePredicateBase*, const mitk::DataNode* = nullptr) const;

  Nodes DirectChildrenOf(const mitk::DataNode* parentNode) const
  {
    return GetNodesInTree(parentNode, true, false);
  }
  Nodes ChildrenOf(const mitk::DataNode* parentNode) const
  {
    return GetNodesInTree(parentNode, false, false);
  }
  Nodes GetNodesInTree(const mitk::DataNode*, const bool, const bool) const;

  void HideAllChildren(const mitk::DataNode*);
  void ShowAllParents(const mitk::DataNode*);
  void ShowAllChildren(const mitk::DataNode*);
  mitk::DataNode* GetParent(const mitk::DataNode*);

  void ChangeParent(mitk::DataNode*, mitk::DataNode*);
  void AddNode(mitk::DataNode*, mitk::DataNode* = nullptr);
  void RemoveNode(const mitk::DataNode*);
  void RemoveAllNodes(mitk::DataNode* parent = nullptr)
  {
    RemoveAll(nullptr, parent);
  }
  void RemoveAll(const char*, const mitk::DataNode* = nullptr);
  void RemoveAllChildren(const mitk::DataNode*);

private:
  mitk::DataStorage* m_DataStorage;
};

void ImekaCommon_EXPORT RemoveNodeFromDataStorage(
  mitk::DataStorage* storage, const mitk::DataNode* node);

} // namespace Imeka

#endif // DATA_MANAGER_HPP_INCLUDED
