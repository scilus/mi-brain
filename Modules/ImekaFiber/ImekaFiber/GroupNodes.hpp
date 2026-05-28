#ifndef IMEKA_FIBER_NODES_HPP_INCLUDED
#define IMEKA_FIBER_NODES_HPP_INCLUDED

#include <mitkDataNode.h>

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT GroupNodes
{
public:
  static const char* CategoryPropertyName;
  static const char* AnatomiesCategoryName;
  static const char* ROIsCategoryName;
  static const char* TractsCategoryName;

  mitk::DataNode::Pointer Anatomies;
  mitk::DataNode::Pointer Tracts;
  mitk::DataNode::Pointer ROIs;

  GroupNodes(Imeka::Callback&, Imeka::DataManager&);

  // void SetROIsCategoryActions(mitk::DataNode* node);
  // void AddVisibilityCallback(mitk::DataNode*) const;

private:
  mitk::DataNode::Pointer NewCategoryNode(const char*) const;

  Imeka::Callback& m_Callback;
  Imeka::DataManager& m_DM;
};

bool ImekaFiber_EXPORT IsAnatomiesCategory(const mitk::DataNode*);
bool ImekaFiber_EXPORT IsROIsCategory(const mitk::DataNode*);
bool ImekaFiber_EXPORT IsTractsCategory(const mitk::DataNode*);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_NODES_HPP_INCLUDED
