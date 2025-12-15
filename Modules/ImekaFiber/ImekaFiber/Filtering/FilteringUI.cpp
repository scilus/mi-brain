
#include "FilteringUI.hpp"

#include <itkCommand.h>

#include <QApplication>
#include <QMessageBox>

#include "ImekaCommon/Property/Property.hpp"
#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/utils.hpp"
#include "FiberBundle/FilteredFiberBundle.hpp"
#include "ImekaWidgets/MITK/DataNodeGroup.hpp"

namespace Imeka
{

namespace Fiber
{

const char* FilteringUI::NbActiveROIsPropertyName = "NbActiveROIs";

FilteringUI::FilteringUI(
  Imeka::Callback& callback,
  Imeka::DataManager& DM,
  FibersColors& fibersColors,
  Filtering& filtering)
  : m_Callback(callback)
  , m_DM(DM)
  , m_Colors(fibersColors)
  , m_Filtering(filtering)
  , m_HasLoadedScene(false)
  , m_IsLoadingScene(false)
  , m_DontProcessAnythingRelatedToFibers(false)
{}

void FilteringUI::SetROIProperties(mitk::DataNode* ROINode) const
{
  // We are not interested in the binary nodes created by MITK's segmentation
  // module. Instead of listing them all, we assume that all "helper objects"
  // are unneeded.
  bool helperObject = false, hiddenObject = false;
  ROINode->GetBoolProperty("helper object", helperObject);
  ROINode->GetBoolProperty("hidden object", hiddenObject);
  if (!helperObject && !hiddenObject)
  {
    ROINode->SetBoolProperty("SelectionROI", true);
  }
}

void FilteringUI::AddActionsToROI(mitk::DataNode* ROINode)
{
  // Rename all associated active ROI nodes when the ROI is renamed
  m_Callback.Add("name", ROINode, [this](mitk::DataNode* ROINode)
  {
    const auto relatedActiveROIs = m_DM.AllNodesRelatedByUUID(
      ROINode, Imeka::DataManager::UUIDPropertyName, "ROI_UUID");
    for (auto activeROINode : relatedActiveROIs)
    {
      activeROINode->SetName(ROINode->GetName());
    }
  });

  // Special case where the ROI is a mask. We need to update the filtering
  // each time the image is updated.
  if (Imeka::Fiber::IsMaskPredicate()->CheckNode(ROINode))
  {
    m_Callback.Add(ROINode, ROINode->GetData(), [this](mitk::DataNode* node)
    {
      const auto image = dynamic_cast<mitk::Image*>(node->GetData());
      if (m_Filtering.UpdateResults(image, true))
      {
        for (auto datasetNode : m_Filtering.LinkedDatasets(image))
        {
          emit RequestUpdateDataset(datasetNode);
        }
        emit RequestGenerateStats();
      }
    });
  }
}

void FilteringUI::AddActionsToTractsCategory(mitk::DataNode* TC)
{
  // This has been added because we want to recalculate the filtering when the
  // user makes the TractsCategory visible again. There's no need to computer
  // anything when it's invisible because it's invisible.
  m_Callback.SetVisibilityCallback(
    TC,
    // OnVisible
    [this, TC]() {
      bool updated = false;
      for (auto datasetNode : m_DM.DirectChildrenOf(TC))
      {
        if (m_Filtering.UpdateResultsLinkedTo(datasetNode))
        {
          emit RequestUpdateDataset(datasetNode);
          updated = true;
        }
      }
      if (updated)
      {
        emit RequestGenerateStats();
      }
    },
    // OnHide
    []() {},
    // After
    []() {},
    m_DM);
}

void FilteringUI::AddActionsToDataset(mitk::DataNode* datasetNode)
{
  // This has been added because we want to recalculate the filtering when the
  // user makes a dataset visible again. There's no need to computer anything
  // when it's invisible because it's invisible.
  m_Callback.SetVisibilityCallback(
    datasetNode,
    // OnVisible
    [this, datasetNode]() {
      if (m_Filtering.UpdateResultsLinkedTo(datasetNode))
      {
        emit RequestUpdateDataset(datasetNode);
        emit RequestGenerateStats();
      }
    },
    // OnHide
    []() {},
    // After
    []() {},
    m_DM);

  m_Callback.Add("CreateNewTractGroup", false, datasetNode,
    [this](mitk::DataNode* datasetNode)
  {
    bool createNewTractGroup;
    datasetNode->GetBoolProperty("CreateNewTractGroup", createNewTractGroup);
    if (!createNewTractGroup) { return; }
    datasetNode->SetBoolProperty("CreateNewTractGroup", false);

    // No need to display the DialogBox if there is no ROI
    const unsigned int nbROIs = GetAllROIs().size();
    if (nbROIs == 0)
    {
      QMessageBox::information(QApplication::activeWindow(), "Imeka",
        "Can't create a tractGroup because there's no ROI.");
      return;
    }

    // If there are only Surfaces and no anat
    if (nbROIs > 0 && nbROIs == GetAllSurfaces().size() && !m_DM.GetAnat())
    {
      QMessageBox::information(QApplication::activeWindow(), "Imeka",
        "Can't create a tractGroup because the only available ROIs are "
        "Surfaces and you must load a reference anatomy to filter with a "
        "surface.");
      return;
    }

    auto TGNode = NewTractGroupNode();
    const bool filteringConfirmed =
      ManageTractGroupSelection(datasetNode, TGNode);
    AddActionsToTractGroup(filteringConfirmed, datasetNode, TGNode);
  });
}

void FilteringUI::AddActionsToTractGroup(
  const bool addVisibilityActions,
  mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode)
{
  if (addVisibilityActions)
  {
    m_Callback.SetVisibilityCallback(
      TGNode,
      // OnVisible
      [this, datasetNode, TGNode](){
        m_Filtering.EnableTractGroup(datasetNode, TGNode);
      },
      // OnHide
      [this, datasetNode, TGNode](){
        m_Filtering.DisableTractGroup(datasetNode, TGNode);
      },
      // After
      [this, datasetNode](){
        emit RequestUpdateDataset(datasetNode);
        emit RequestGenerateStats();
      }, m_DM);
  }

  m_Callback.Add("EditActiveROIs", false, TGNode,
    [this, datasetNode](mitk::DataNode* TGNode)
  {
    bool editActiveROIs;
    TGNode->GetBoolProperty("EditActiveROIs", editActiveROIs);
    if (!editActiveROIs) { return; }

    if (ManageTractGroupSelection(datasetNode, TGNode))
    {
      TGNode->SetBoolProperty("visible", true);
    }
    TGNode->SetBoolProperty("EditActiveROIs", false);
  });

  m_Callback.Add("Duplicate", false, TGNode,
    [this, datasetNode](mitk::DataNode* TGNode)
  {
    bool duplicate;
    TGNode->GetBoolProperty("Duplicate", duplicate);
    if (!duplicate) { return; }
    TGNode->SetBoolProperty("Duplicate", false);

    auto newTGNode = NewTractGroupNode("Dup " + TGNode->GetName());
    AddActionsToTractGroup(true, datasetNode, newTGNode);
    // Color is handled in DuplicateDataset
    newTGNode->SetVisibility(false);
    m_DM.AddNode(newTGNode, datasetNode);

    m_Callback.AllowOneMoreLevelOfCallback();
    auto TG = m_Filtering.DuplicateDataset(datasetNode, TGNode, newTGNode);
    TG.modified = true;

    for (auto& ROI_Line : TG.lines)
    {
      const auto ROI = const_cast<mitk::BaseData*>(ROI_Line.first);
      auto& line = ROI_Line.second;

      line.enabled = false;
      auto newActiveROINode = NewActiveROINode(
        m_DM.GetNodeContainingThis(ROI), line);
      AddActionsToActiveROI(datasetNode, newTGNode, newActiveROINode);
      newActiveROINode->SetVisibility(false);
      line.activeROINode = newActiveROINode;
      m_DM.AddNode(newActiveROINode, newTGNode);
    }

    Imeka::Property::Set(
      newTGNode, NbActiveROIsPropertyName,
      m_DM.DirectChildrenOf(TGNode).size());
  });
}

void FilteringUI::AddActionsToActiveROI(
  mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode,
  mitk::DataNode* activeROI)
{
  m_Callback.SetVisibilityCallback(
    activeROI,
    // OnVisible
    [this, datasetNode, TGNode, activeROI]()
    {
      // We are getting the ROI from the activeROI and not from the caller
      // because it could be a different ROI (from a surface update).
      auto ROI = GetROILinkedTo(activeROI)->GetData();
      m_Filtering.EnableROIInTractGroup(datasetNode, TGNode, ROI);
    },
    // OnHide
    [this, datasetNode, TGNode, activeROI]()
    {
      auto ROI = GetROILinkedTo(activeROI)->GetData();
      m_Filtering.DisableROIInTractGroup(datasetNode, TGNode, ROI);
    },
    // After
    [this, datasetNode]()
    {
      emit RequestUpdateDataset(datasetNode);
      emit RequestGenerateStats();
    }
    , m_DM);
}

bool FilteringUI::ManageTractGroupSelection(
  mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode)
{
  // No need to display the DialogBox if there is no ROI
  const auto allROIs = GetAllROIs();
  if (allROIs.empty()) { return false; }

  auto& TG = m_Filtering.GetTractGroup(datasetNode, TGNode);
  const bool hasAnat = m_DM.GetAnat() != nullptr;

  // There shouldn't be a specific branch for test but we can't instanciate a
  // QDialog in a test environment.
  if (QApplication::activeWindow() == nullptr)
  {
    ActiveROIsDialogBox::AutoSetup(TG, allROIs, hasAnat);
  }
  else
  {
    // Keep a copy of TG.lines because On cancel, we need to revert the changes
    auto linesCopy = TG.lines;

    ActiveROIsDialogBox activeROIsDialog(QApplication::activeWindow());
    const bool userDidSomething = activeROIsDialog.GenerateDialogBox(
      TG, TGNode, allROIs, hasAnat);
    if (!userDidSomething)
    {
      // We need to delete the newly created TG if it's an ADD and not an EDIT.
      if (m_DM.GetAllNodes(TGNode).empty())
      {
        m_Filtering.DeleteTractGroup(datasetNode, TGNode);
      }
      else
      {
        // If the TG is still alive, revert its selection
        TG.lines = linesCopy;
      }
      return false;
    }
  }

  m_Filtering.UpdateTractGroup(datasetNode, TG);

  // Keep a copy of TG.lines because RemoveAllChildren clears it
  auto linesCopy = TG.lines;

  // We delete and add back all the ROI nodes associated with this TG
  m_DontProcessAnythingRelatedToFibers = true;
  m_DM.RemoveAllChildren(TGNode);
  TG.lines.clear();
  m_DontProcessAnythingRelatedToFibers = false;

  const bool nothingIsEnabled = [allROIs, linesCopy](){
    bool enabed = false;
    for (const auto ROINode : allROIs)
    {
      const auto ROI = ROINode->GetData();
      const auto& line = linesCopy.at(ROI);
      enabed |= line.enabled;
    }
    return !enabed;
  }();
  if (nothingIsEnabled) { return false; }

  m_DontProcessAnythingRelatedToFibers = true;
  if (!m_DM.GetDataStorage()->Exists(TGNode))
  {
    m_DM.AddNode(TGNode, datasetNode);
  }
  m_DM.ShowAllParents(TGNode);

  bool wasUpdated = false;
  for (auto ROINode : allROIs)
  {
    // Must remove actions because we check for any modification on the inner
    // base data
    m_Callback.Remove(ROINode);
    AddActionsToROI(ROINode);

    const auto ROI = ROINode->GetData();
    if (linesCopy.at(ROI).enabled)
    {
      auto& line = TG.lines[ROI] = linesCopy.at(ROI);
      auto newActiveROI = NewActiveROINode(ROINode, line);
      m_DM.AddNode(newActiveROI, const_cast<mitk::DataNode*>(TGNode));
      line.activeROINode = newActiveROI;

      AddActionsToActiveROI(datasetNode, TGNode, newActiveROI);
      wasUpdated = true;
    }
  }
  m_DontProcessAnythingRelatedToFibers = false;

  if (wasUpdated)
  {
    Imeka::Property::Set(
      TGNode, NbActiveROIsPropertyName, m_DM.DirectChildrenOf(TGNode).size());

    emit RequestUpdateDataset(datasetNode);

    const auto fiber =
      dynamic_cast<mitk::FilteredFiberBundle*>(datasetNode->GetData());
    fiber->CalculateStatsUsingVisibility();

    emit RequestGenerateStats();
  }

  return true;
}

void FilteringUI::LoadingSceneCheck(const mitk::DataNode* TGNode)
{
  if (m_IsLoadingScene) { return; }

  auto datasetNode = m_DM.GetParent(TGNode);
  const auto& datasetInfo = m_Filtering.DatasetInfo()[datasetNode];
  if (datasetInfo.tractGroups.count(TGNode) == 0)
  {
    m_IsLoadingScene = true;
  }
  else
  {
    unsigned int requiredNbActiveROIs = 0;
    const bool hasValue = TGNode->GetPropertyValue(
      NbActiveROIsPropertyName, requiredNbActiveROIs);
    const auto nbChildren = m_DM.DirectChildrenOf(TGNode).size();
    if (hasValue && nbChildren == requiredNbActiveROIs)
    {
      m_IsLoadingScene = true;
    }
  }
  m_HasLoadedScene |= m_IsLoadingScene;
}

bool FilteringUI::FinishedLoading()
{
  // Filtering assume a correct loading order: Dataset, ROI, tractGroup and
  // ActiveROI. The scene loading order is random.
  // This method checks if there's still some missing nodes and does nothing
  // if it's the case. When all nodes are loaded, it puts the object in a
  // right state to be able to filter.
  if (!m_IsLoadingScene) { return false; }

  // Make sure there's no TG without children
  for (auto TGNode : m_DM.GetAll(GetTractGroupPredicate()))
  {
    unsigned int requiredNbActiveROIs = 0;
    TGNode->GetPropertyValue(NbActiveROIsPropertyName, requiredNbActiveROIs);
    if (m_DM.DirectChildrenOf(TGNode).size() != requiredNbActiveROIs)
    {
      return false;
    }
  }

  const auto allActiveROIs = m_DM.GetAll(GetActiveROIPredicate());
  if (allActiveROIs.empty()) { return false; }

  // Return if there's still one missing node (scene still loading)
  for (auto activeROINode : allActiveROIs)
  {
    const auto node = GetROILinkedTo(activeROINode);
    if (!node) { return false; }
  }

  // The last missing datanode was just loaded.
  // Not we put the Filtering object into the right state

  std::map<mitk::DataNode*, Nodes> datasetsAndTG;
  for (auto activeROINode : allActiveROIs)
  {
    const auto TGNode = m_DM.GetParent(activeROINode);
    const auto datasetNode = m_DM.GetParent(TGNode);
    const auto ROI = GetROILinkedTo(activeROINode)->GetData();

    auto& datasetInfo = m_Filtering.DatasetInfo()[datasetNode];
    auto& TG = datasetInfo.tractGroups[TGNode];
    auto& line = TG.lines[ROI];

    // Keep info for later because we want to add the action only once
    datasetsAndTG[datasetNode].push_back(TGNode);
    AddActionsToActiveROI(datasetNode, TGNode, activeROINode);

    line.activeROINode = activeROINode;
    line.enabled = activeROINode->IsVisible(nullptr);
    int mode = -1;
    activeROINode->GetIntProperty("SegmentSelection", mode);
    line.mode = static_cast<SelectionMode>(mode);
    activeROINode->GetBoolProperty("NOT", line.isNot);

    auto& results = datasetInfo.results[ROI]; // Create it
    m_Filtering.RealFiltering(datasetInfo, ROI, results, true);
  }

  for (auto ROINode : GetAllROIs())
  {
    AddActionsToROI(ROINode);
  }
  for (const auto p : datasetsAndTG)
  {
    for (auto TGNode : p.second)
    {
      AddActionsToTractGroup(true, p.first, TGNode);
    }
  }

  // It's the responsability of the caller to update and draw the
  // fibers, because we can't do it here.
  m_IsLoadingScene = false;
  return true;
}

bool FilteringUI::SurfaceNodeUpdated(
  const mitk::DataNode* node,
  mitk::Surface* newSurface)
{
  const auto allRelatedNodes = m_DM.AllNodesRelatedByUUID(
    node, "MASK_UUID", "MASK_UUID");
  if (allRelatedNodes.size() <= 1) { return false; }

  mitk::DataNode* oldSurfaceNode = nullptr;
  if (node == allRelatedNodes[0]) { oldSurfaceNode = allRelatedNodes[1]; }
  else { oldSurfaceNode = allRelatedNodes[0]; }

  // Replace the old surface by the new one but keep the old pointer
  const auto oldSurface =
    dynamic_cast<mitk::Surface*>(oldSurfaceNode->GetData());
  oldSurfaceNode->SetData(newSurface);

  // Then update the filtering
  const auto linkedDatasets = m_Filtering.ReplaceROI(oldSurface, newSurface);
  if (!linkedDatasets.empty())
  {
    for (const auto datasetNode : linkedDatasets)
    {
      emit RequestUpdateDataset(datasetNode);
    }
    emit RequestGenerateStats();
  }

  // Tell the caller that we replaced the surface so it can delete the node.
  return true;
}

mitk::DataNode* FilteringUI::GetROILinkedTo(
  const mitk::DataNode* activeROINode) const
{
  const auto allROIs = m_DM.AllNodesRelatedByUUID(
    activeROINode, "ROI_UUID", Imeka::DataManager::UUIDPropertyName);
  if (allROIs.empty()) { return nullptr; }
  return allROIs[0];
}

mitk::DataNode::Pointer FilteringUI::NewActiveROINode(
  const mitk::DataNode* ROINode,
  const TractGroup::Line& line)
{
  auto newNode = mitk::DataNode::New();
  newNode->SetData(Imeka::Common::DataNodeGroup::New());
  newNode->SetName(ROINode->GetName());
  newNode->SetStringProperty(GroupPropertyName, ActiveROIName);
  m_DM.CopyUUID(ROINode, newNode, "ROI_UUID");
  newNode->SetBoolProperty("fixedName", true);
  newNode->SetBoolProperty("visible", true);
  newNode->SetIntProperty("SegmentSelection", line.mode);
  newNode->SetBoolProperty("NOT", line.isNot);
  return newNode;
}

mitk::DataNode::Pointer FilteringUI::NewTractGroupNode(
  const std::string& nodeName) const
{
  auto node = mitk::DataNode::New();
  node->SetData(Imeka::Common::DataNodeGroup::New());
  node->SetName(nodeName);
  node->SetBoolProperty("visible", true);
  node->SetStringProperty(GroupPropertyName, TractGroupName);
  node->SetBoolProperty("EditActiveROIs", false);
  return node;
}

std::set<mitk::DataNode*> FilteringUI::FilteringNodeRemoved(
  const mitk::DataNode* node)
{
  if (IsTractGroup(node))
  {
    const auto datasetNode = m_DM.GetParent(node);
    m_Filtering.DeleteTractGroup(datasetNode, node);
    for (const auto activeROINode : m_DM.ChildrenOf(node))
    {
      m_Callback.Remove(activeROINode);
      m_DM.RemoveNode(activeROINode);
    }
    return{ datasetNode };
  }

  if (IsActiveROI(node))
  {
    auto TGNode = m_DM.GetParent(node);
    const auto datasetNode = m_DM.GetParent(TGNode);
    if (!m_DontProcessAnythingRelatedToFibers)
    {
      // == 1 for empty because node is not yet deleted
      const bool isEmptyNow = m_DM.DirectChildrenOf(TGNode).size() == 1;
      if (isEmptyNow)
      {
        m_Callback.Remove(TGNode);
        m_DM.RemoveNode(TGNode);
      }
      m_Filtering.DeleteROIInTractGroup(
        datasetNode, TGNode, GetROILinkedTo(node)->GetData());
      if (isEmptyNow) { TGNode = nullptr; }
    }

    // Possibly deleted by the condition above
    if (TGNode)
    {
      // - 1 because the ActiveROI is not deleted yet
      Imeka::Property::Set(
        TGNode, NbActiveROIsPropertyName,
        m_DM.DirectChildrenOf(TGNode).size() - 1);
    }

    return{ datasetNode };
  }

  std::set<mitk::DataNode*> nodesToUpdate;
  const auto activeROINodes = m_Filtering.DeleteROI(node->GetData());
  for (const auto node : activeROINodes)
  {
    const auto TGNode = m_DM.GetParent(node);

    m_Callback.Remove(node);
    m_DM.RemoveNode(node);

    nodesToUpdate.insert(m_DM.GetParent(TGNode));
    const unsigned int nbActiveROIs = m_DM.DirectChildrenOf(TGNode).size();
    if (nbActiveROIs == 0)
    {
      m_Callback.Remove(TGNode);
      m_DM.RemoveNode(TGNode);
    }
    else
    {
      Imeka::Property::Set(TGNode, NbActiveROIsPropertyName, nbActiveROIs);
    }
  }

  return nodesToUpdate;
}

Nodes FilteringUI::GetAllROIs() const
{
  return m_DM.GetAll(Imeka::Predicate::Property("SelectionROI", true));
}

} // namespace Fiber

} // namespace Imeka
