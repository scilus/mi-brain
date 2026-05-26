#include "GroupNodes.hpp"

#include <mitkDataNode.h>

#include "ImekaGeometry/ViewUtils.hpp"
#include "Mapper/Mappers2DSettingsWidget.hpp"
#include "ImekaWidgets/MITK/DataNodeGroup.hpp"

namespace Imeka
{

namespace Fiber
{

const char* GroupNodes::CategoryPropertyName = "CategoryGroup";
const char* GroupNodes::AnatomiesCategoryName = "Anatomies";
const char* GroupNodes::ROIsCategoryName = "ROIs";
const char* GroupNodes::TractsCategoryName = "Tracts";

// When created, the group nodes are added to the DM and are empty.
// All callbacks are added in FibersManager::GroupAdded, so they are
// added both when the group nodes are created and when they are loaded from a scene.
GroupNodes::GroupNodes(
  ROIAction ROICallback,
  Imeka::Callback& callback,
  Imeka::DataManager& dm)
  : m_ROIAction(ROICallback)
  , m_Callback(callback)
  , m_DM(dm)
{
  Tracts = NewCategoryNode(TractsCategoryName);
  ROIs = NewCategoryNode(ROIsCategoryName);
  Anatomies = NewCategoryNode(AnatomiesCategoryName);
}

bool GroupNodes::UpdateGroupIfRequired(mitk::DataNode* node)
{
  // Did we load a scene? If so, the category nodes are already in the DM
  // so we must delete them before adding the new ones.
  std::string categoryGroupName = "";
  if (node->GetStringProperty(CategoryPropertyName, categoryGroupName))
  {
    if (categoryGroupName == "Anatomies")
    {
      m_Callback.Remove(Anatomies);
      m_DM.RemoveNode(Anatomies);
      Anatomies = node;
    }
    else if (categoryGroupName == "Tracts")
    {
      m_Callback.Remove(Tracts);
      m_DM.RemoveNode(Tracts);
      Tracts = node;
    }
    else if (categoryGroupName == "ROIs")
    {
      m_Callback.Remove(ROIs);
      m_DM.RemoveNode(ROIs);
      ROIs = node;

      m_Callback.Add("Create", "", ROIs, m_ROIAction);
    }
    AddVisibilityCallback(node);
    return true;
  }
  return false;
}

mitk::DataNode::Pointer GroupNodes::NewCategoryNode(const char* name) const
{
  // Node properties
  auto node = mitk::DataNode::New();
  node->SetName(name);
  node->SetData(Imeka::Common::DataNodeGroup::New());
  node->SetBoolProperty("visible", true);
  node->SetStringProperty(CategoryPropertyName, name);
  node->SetBoolProperty("undeletable", true);
  node->SetBoolProperty("fixedName", true);
  node->SetBoolProperty("includeInBoundingBox", false);

  m_DM.AddNode(node);
  return node;
}

bool ImekaFiber_EXPORT IsAnatomiesCategory(const mitk::DataNode* node)
{
  std::string categoryGroup;
  node->GetStringProperty(GroupNodes::CategoryPropertyName, categoryGroup);
  return categoryGroup == GroupNodes::AnatomiesCategoryName;
}

bool ImekaFiber_EXPORT IsROIsCategory(const mitk::DataNode* node)
{
  std::string categoryGroup;
  node->GetStringProperty(GroupNodes::CategoryPropertyName, categoryGroup);
  return categoryGroup == GroupNodes::ROIsCategoryName;
}

bool ImekaFiber_EXPORT IsTractsCategory(const mitk::DataNode* node)
{
  std::string categoryGroup;
  node->GetStringProperty(GroupNodes::CategoryPropertyName, categoryGroup);
  return categoryGroup == GroupNodes::TractsCategoryName;
}

// This function adds the create callback to the ROIs category, so it must be called for the ROIs category node.
// The function exists here because ROIAction is defined in this class, and I don't understand how or where it is
// created, so I don't know how to pass it to FibersManager.
void GroupNodes::SetROIsCategoryActions(mitk::DataNode* node){
  m_Callback.Add("Create", "", node, m_ROIAction);
}

// this function being here is really not ideal but it's the simplest
// way to try to centralize the visibility callback logic without having
// to untangle the whole code base to find every visibility callback or other callbacks.

// This function add a visibility callback to the given node.
void GroupNodes::AddVisibilityCallback(
  mitk::DataNode* groupNode) const
{
  if (groupNode->GetName() == TractsCategoryName)
  {
    // All callbacks are one level deep so the tracts visibility won't trigger
    // their own callbacks. Adding more "deepness" is quite complex so we
    // simply handle it here with a specific callback.
    m_Callback.AddVisibilityCallback(
      groupNode,
      []() {},
      []() {},
      [this, groupNode]() {
        const auto visible = groupNode->IsVisible(nullptr)
          && Mappers2DSettingsWidget::Instance->IsEnabled();
        const auto renderers = Imeka::View::Get2DRenderers();
        for (auto node : m_DM.DirectChildrenOf(groupNode))
        {
          for (auto renderer : renderers)
          {
            node->SetBoolProperty("visible", visible, renderer);
          }
        }
      },
      m_DM);
  }
  else
  {
    m_Callback.AddVisibilityCallback(groupNode, m_DM);
  }
}

} // namespace Fiber

} // namespace Imeka
