#include "GroupNodeManager.hpp"

#include "ImekaGeometry/ViewUtils.hpp"
#include "Mapper/Mappers2DSettingsWidget.hpp"
#include "Saver.hpp"

namespace Imeka
{
namespace Fiber
{
    GroupNodeManager::GroupNodeManager(
        Imeka::Callback& callback,
        Imeka::DataManager& dm,
        FibersColors& fibersColors,
        FilteringUI& filteringUI,
        GroupNodes& groups,
        Callback::CallbackFunction roiCreate
    )
        : m_Callback(callback)
        , m_DM(dm)
        , m_FibersColors(fibersColors)
        , m_FilteringUI(filteringUI)
        , m_Groups(groups)
        , m_ROICreate(roiCreate){
            // We no longer want the nodes to be present on startup.
            // Instead, we add them when a file is loaded.
            // That is done from the FibersManager class.
            
            // InitializeGroupNode(m_Groups.Anatomies);
            // InitializeGroupNode(m_Groups.ROIs);
            // InitializeGroupNode(m_Groups.Tracts);
        }

    void GroupNodeManager::InitializeGroupNode(mitk::DataNode* node) {
        if(!IsGroupNodePresent(node)){
            m_DM.AddNode(node);
            std::string categoryGroupName = "";
            node->GetStringProperty(GroupNodes::CategoryPropertyName, categoryGroupName);
            if (categoryGroupName == GroupNodes::AnatomiesCategoryName)
            {
                SetupAnatomiesGroup(node);
            }
            else if (categoryGroupName == GroupNodes::TractsCategoryName)
            {
                SetupTractsGroup(node);
            }
            else if (categoryGroupName == GroupNodes::ROIsCategoryName)
            {
                SetupROIsGroup(node);
            }
        }
    }

    void GroupNodeManager::EnsureAllGroupsExist(){
        InitializeGroupNode(m_Groups.Anatomies);
        InitializeGroupNode(m_Groups.ROIs);
        InitializeGroupNode(m_Groups.Tracts);
    }

    bool GroupNodeManager::SetupGroupIfRequired(mitk::DataNode* node) { 
        // Did we load a scene? If so, the category nodes are already in the DM
        // so we must delete them before adding the new ones.
        std::string categoryGroupName = "";
        // checks if the node is a group node by checking if it has the category property.
        if (node->GetStringProperty(GroupNodes::CategoryPropertyName, categoryGroupName))
        {
            if (categoryGroupName == GroupNodes::AnatomiesCategoryName)
            {
                // Remove the old group node and its callbacks
                m_Callback.Remove(m_Groups.Anatomies);
                m_DM.RemoveNode(m_Groups.Anatomies);
                // Replace the old pointer with the new one
                m_Groups.Anatomies = node;
            }
            else if (categoryGroupName == GroupNodes::TractsCategoryName)
            {
                m_Callback.Remove(m_Groups.Tracts);
                m_DM.RemoveNode(m_Groups.Tracts);
                m_Groups.Tracts = node;
            }
            else if (categoryGroupName == GroupNodes::ROIsCategoryName)
            {
                m_Callback.Remove(m_Groups.ROIs);
                m_DM.RemoveNode(m_Groups.ROIs);
                m_Groups.ROIs = node;
            }
            InitializeGroupNode(node);
            return true;
        }
        return false;
     }

    void GroupNodeManager::SetupAnatomiesGroup(mitk::DataNode* node){
        m_Callback.AddVisibilityCallback(node, m_DM);
    }

    void GroupNodeManager::SetupTractsGroup(mitk::DataNode* node){
        m_Callback.AddVisibilityCallback(
            node,
            []() {},
            []() {},
            [this, node]() {
                const auto visible = node->IsVisible(nullptr)
                    && Mappers2DSettingsWidget::Instance->IsEnabled();
                const auto renderers = Imeka::View::Get2DRenderers();
                for (auto childnode : m_DM.DirectChildrenOf(node))
                {
                    for (auto renderer : renderers)
                    {
                        childnode->SetBoolProperty("visible", visible, renderer);
                    }
                }
            },
            m_DM
        );

        m_FibersColors.SetTractsCategoryActions(node);
        m_FilteringUI.AddActionsToTractsCategory(node);
    
        m_Callback.Add("Save", "", node, [this](mitk::DataNode* node)
        {
          std::string saveTo = "";
          if (!node->GetStringProperty("Save", saveTo) || saveTo.empty()) { return; }
          node->SetStringProperty("Save", "");

          Imeka::Fiber::Saver::Instance().Save(saveTo == "dm", m_DM);
        });
    }
    
    void GroupNodeManager::SetupROIsGroup(mitk::DataNode* node){
        m_Callback.AddVisibilityCallback(node, m_DM);
        m_FibersColors.SetROIsCategoryActions(node);
        m_Callback.Add("Create", "", node, m_ROICreate);
    }

    // Returns True if the node is already present in the DataManager
    bool GroupNodeManager::IsGroupNodePresent(mitk::DataNode* node){
        return m_DM.GetDataStorage()->Exists(node);
    }
}
}