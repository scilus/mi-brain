#include "FibersManager.hpp"

#include <mitkVtkResliceInterpolationProperty.h>

#include "ImekaCommon/Colors.hpp"
#include "ImekaCommon/Predicate.hpp"
#include "ImekaGeometry/ViewUtils.hpp"
#include "Mapper/Mappers2DSettingsWidget.hpp"
#include "Saver.hpp"
#include "StreamlinesCutter.hpp"
#include "utils.hpp"

namespace Imeka
{

namespace Fiber
{

FibersManager::FibersManager(
  Imeka::DataManager& DM,
  Imeka::Callback& callback,
  GroupNodes& groups,
  Callback::CallbackFunction roiCreate)
  : m_DM(DM)
  , m_Callback(callback)
  , m_FibersColors(m_DM, m_Callback, m_FibersNodeData)
  , m_SurfaceColors(m_DM, m_Callback, m_FibersNodeData)
  , m_Groups(groups)
  , m_AnatNode(nullptr)
  , m_Filtering(m_FibersNodeData)
  , m_FilteringUI(m_Callback, m_DM, m_FibersColors, m_Filtering)
  , m_GroupNodeManager(m_Callback, m_DM, m_FibersColors, m_FilteringUI, m_Groups, roiCreate)
{
  Saver::Instance().AddNodes(&m_FibersNodeData, false);

  // m_GroupNodeManager(
    // m_Callback, m_DM, m_FibersColors, m_FilteringUI, m_Groups, roiCreate);

  connect(&m_FilteringUI, &FilteringUI::RequestUpdateDataset,
    [this](mitk::DataNode* datasetNode)
  {
    ComputeFibersVisibility(datasetNode, true);
  });
  connect(&m_FilteringUI, &FilteringUI::RequestGenerateStats,
    this, &FibersManager::UpdateAlltats);
}

void FibersManager::SetAnat(mitk::DataNode* node, mitk::Image* anat)
{
  m_AnatNode = node;
  m_FibersColors.SetAnat(node);
  m_SurfaceColors.SetAnat(node);
  m_DM.SetAnat(node);
  m_Filtering.SetAnat(anat);
}

void FibersManager::RemoveFibersNode(mitk::DataNode* node)
{
  auto it = m_FibersNodeData.find(node);
  if (it != m_FibersNodeData.end())
  {
    m_FibersColors.FiberDeleted(node);
    m_Filtering.DeleteDataset(node);
    m_FibersNodeData.erase(it);
  }
}

// This function is essentially a wrapper around GroupNodeManager::InitializeGroupNode
void FibersManager::GroupAdded(mitk::DataNode* node)
{
  m_GroupNodeManager.InitializeGroupNode(node);
}

// This is also a wrapper, which came from GroupNodes to move the logic into GroupNodeManager.
bool FibersManager::UpdateGroupIfRequired(mitk::DataNode* node){
  return m_GroupNodeManager.SetupGroupIfRequired(node);
}

void FibersManager::NodeAdded(mitk::DataNode* node)
{
  m_DM.GiveUUID(node);

  auto selectionObject = dynamic_cast<SelectionObject*>(node->GetData());
  if (selectionObject)
  {
    ROIAdded(node);
  }
  else if (auto surface = dynamic_cast<mitk::Surface*>(node->GetData()))
  {
    SurfaceAdded(node, surface);
  }
  else if (auto image = dynamic_cast<mitk::Image*>(node->GetData()))
  {
    bool binary = false;
    node->GetBoolProperty("binary", binary);
    if (binary || Imeka::Fiber::IsLabelsImage(image))
    {
      if (binary)
      {
        node->SetBoolProperty("segmentation", true);
        node->SetBoolProperty("org.mitk.views.segmentation.ismask", true);
      }

      MITK_INFO << "Binary or labels image loaded; setting interpolation to NN.\n";
      auto interpolation =
        dynamic_cast<mitk::VtkResliceInterpolationProperty*>(
          node->GetProperty("reslice interpolation"));
      interpolation->SetInterpolationToNearest();
    }
    if (Imeka::Fiber::GetROIPredicate()->CheckNode(node))
    {
      if (!m_DM.GetDataStorage()->Exists(m_Groups.ROIs)) { 
        MITK_INFO << "ROIs group missing, adding it back.\n";
        GroupAdded(m_Groups.ROIs);
      }
      ROIAdded(node);
    }
    else
    {
      if (!m_DM.GetDataStorage()->Exists(m_Groups.Anatomies)) { 
        MITK_INFO << "Anatomies group missing, adding it back.\n";
        GroupAdded(m_Groups.Anatomies);
      }
      m_DM.ChangeParent(node, m_Groups.Anatomies);
    }
  }
  else if (
    auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData()))
  {
    bool helperObject = false;
    node->GetBoolProperty("helper object", helperObject);
    if (!helperObject) // Not RTT
    {
      MITK_INFO << "FibersManager: Adding fibers node " << node->GetName() << "\n";
      if (!m_DM.GetDataStorage()->Exists(m_Groups.Tracts))
      {
        MITK_INFO << "FibersManager: Tracts group missing, adding it back.\n";
        GroupAdded(m_Groups.Tracts);
      }
      m_DM.ChangeParent(node, m_Groups.Tracts);
      FibersAdded(node, fiber);

      m_Callback.Add("Save", "", node, [this](mitk::DataNode* node)
      {
        std::string saveTo = "";
        if (!node->GetStringProperty("Save", saveTo) || saveTo.empty())
        {
          return;
        }
        node->SetStringProperty("Save", "");

        Imeka::Fiber::Saver::Instance().Save(node, saveTo == "dm", m_DM);
      });
    }
  }
  else if (Imeka::Fiber::IsTractGroup(node))
  {
    // Saving a TG means saving only the visible streamlines in this TG, not
    // in the whole dataset. It's different only when there's more than one TG.
    m_Callback.Add("Save", "", node, [this](mitk::DataNode* TGNode)
    {
      std::string saveTo = "";
      if (!TGNode->GetStringProperty("Save", saveTo) || saveTo.empty())
      {
        return;
      }
      TGNode->SetStringProperty("Save", "");

      const auto datasetNode = m_DM.GetParent(TGNode);
      Imeka::Fiber::Saver::Instance().Save(
        datasetNode, TGNode, saveTo == "dm", m_DM);
    });

    m_Callback.Add("Cut", "", node, [this](mitk::DataNode* node){
      CutStreamlines(node);
    });

    // We check if we are loading a scene each time a TG is added
    m_FilteringUI.LoadingSceneCheck(node);
  }

  if (m_FilteringUI.FinishedLoading())
  {
    UpdateAllFibers();
  }
}

void FibersManager::SurfaceAdded(
  mitk::DataNode* node,
  mitk::Surface* surface)
{
  m_SurfaceColors.SetSurfaceActions(node);
  for (auto renderer : Imeka::View::Get2DRenderers())
  {
    node->SetFloatProperty("line width", 7.0, renderer);
  }

  const auto parent = m_DM.GetParent(node);
  if (parent && parent != m_Groups.ROIs)
  {
    /* If `parent` is a binary image, then the surface was created by the
    "Create [Smoothed] Segmentation" menu. We want to keep track of this
    because we want to update the filtering when another surface is created
    from the same mask */
    bool binary = false;
    parent->GetBoolProperty("binary", binary);
    if (binary)
    {
      m_DM.CopyUUID(parent, node, "MASK_UUID");
      if (m_FilteringUI.SurfaceNodeUpdated(node, surface))
      {
        node->SetData(nullptr);
        m_DM.RemoveNode(node);
        return;
      }
    }
  }

  ROIAdded(node);
}

void FibersManager::FibersAdded(
  mitk::DataNode* node,
  mitk::FilteredFiberBundle* fiber)
{
  SetupNodeDataAndMappers(node, fiber, m_FibersNodeData[node]);
  m_FibersColors.SetFibersActions(node);

  m_Callback.Add("MirrorFibers", -1, node, [this, fiber](mitk::DataNode* node)
  {
    int axis = -1;
    node->GetIntProperty("MirrorFibers", axis);
    if (axis == -1) { return; }

    MirrorFibers(node, fiber, axis);
    node->SetIntProperty("MirrorFibers", -1);
  });

  m_Callback.Add("ShuffleFibers", false, node, [this, fiber](mitk::DataNode* node)
  {
    bool shuffleFibers = false;
    node->GetBoolProperty("ShuffleFibers", shuffleFibers);
    if (!shuffleFibers) { return; }

    ShuffleFibers(node, fiber);
    node->SetBoolProperty("ShuffleFibers", false);
  });

  m_Callback.Add("Cut", "", node, [this](mitk::DataNode* node){
    CutStreamlines(node);
  });

  m_Filtering.AddDataset(node, fiber);

  // Centering and updating views
  mitk::RenderingManager::GetInstance()->InitializeViews(fiber->GetGeometry());
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  // Then we recompute the total visibility with the new fiber selections.
  ComputeFibersVisibility(node, true);
  fiber->CalculateStatsUsingVisibility();
  emit DisplayStats();

  m_Callback.AddVisibilityCallback(
    node,
    [](){},
    [](){},
    [](){
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    }, m_DM);

  m_FilteringUI.AddActionsToDataset(node);
}

void FibersManager::ROIAdded(mitk::DataNode* node)
{
  if (m_DM.GetParent(node) != m_Groups.ROIs)
  {
    m_DM.ChangeParent(node, m_Groups.ROIs);
  }

  m_FilteringUI.SetROIProperties(node);

  // ROIs are always on top.
  node->SetIntProperty("layer", 899);
  node->SetBoolProperty("fixedLayer", true);
}

void FibersManager::NodeRemoved(mitk::DataNode* node)
{
  if (dynamic_cast<mitk::FiberBundle*>(node->GetData()))
  {
    RemoveFibersNode(node);
    m_DM.RemoveAllChildren(node);
  }
  else if (dynamic_cast<mitk::Image*>(node->GetData()))
  {
    // Some hidden nodes, not going through the NodeAdded() function, exist
    // for segmentation image, causing a problem with the Anatomy count.
    for (auto subNode : m_DM.DirectChildrenOf(node))
    {
      // Only delete the segmentation image
      if (dynamic_cast<mitk::Image*>(subNode->GetData()))
      {
        m_DM.RemoveNode(subNode);
      }
    }
  }

  const auto nodesToUpdate = m_FilteringUI.FilteringNodeRemoved(node);

  // Actually remove the node from MITK DM and memory. We do this because we
  // don't want ComputeFibersVisibility to count the node that we just deleted.
  m_DM.RemoveNode(node);

  for (auto n : nodesToUpdate)
  {
    ComputeFibersVisibility(n, true);
  }
}

void FibersManager::SelectionObjectMoved(SelectionObject* SO, const bool precise)
{
  if (!m_Filtering.UpdateResults(SO, precise)) { return; }

  // We compute the streamline selection for each fiber
  // dataset and compute visibility
  for (const auto fiberNode : m_Filtering.LinkedDatasets(SO))
  {
    ComputeFibersVisibility(fiberNode, precise);
    if (precise)
    {
      auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(fiberNode->GetData());
      fiber->CalculateStatsUsingVisibility();
    }
  }

  if (precise) { emit DisplayStats(); }
}

void FibersManager::SelectionObjectReleased(SelectionObject* SO)
{
  if (!SO) { return; }

  SelectionObjectMoved(SO, true);
  emit DisplayStats();
}

ConstNodes FibersManager::FibersNodesToTransform(
  const ConstNodes& selectedFiberBundles) const
{
  return GetFiberNodesToTransform(selectedFiberBundles, m_FibersNodeData);
}

void FibersManager::UpdateAllFibers()
{
  const auto end = m_FibersNodeData.cend();
  for (auto it = m_FibersNodeData.cbegin(); it != end; ++it)
  {
    ComputeFibersVisibility(it.key(), true);
  }
  UpdateAlltats();
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void FibersManager::ComputeFibersVisibility(
  const mitk::DataNode* datasetNode,
  const bool precise)
{
  if (m_FilteringUI.AsksMeToStop()) { return; }

  auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(datasetNode->GetData());
  if (m_Filtering.FilterDataset(datasetNode, fiber, precise))
  {
    m_FibersNodeData[datasetNode].UpdateIndices();
  }
}

void FibersManager::UpdateAlltats()
{
  const auto end = m_FibersNodeData.cend();
  for (auto it = m_FibersNodeData.cbegin(); it != end; ++it)
  {
    auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(it.key()->GetData());
    fiber->CalculateStatsUsingVisibility();
  }
  emit DisplayStats();
}

void FibersManager::CutStreamlines(mitk::DataNode* targetNode)
{
  const auto ROINode = m_DM.GetFirst(
    Imeka::Predicate::Property("CutWithMeSenpai!", true));
  std::string cutWhat;
  targetNode->GetStringProperty("Cut", cutWhat);
  const bool fromOutside = cutWhat == "Outside";
  CutStreamlines(ROINode, targetNode, fromOutside);

  targetNode->SetStringProperty("Cut", "");
}

void FibersManager::CutStreamlines(
  const mitk::DataNode* ROINode,
  const mitk::DataNode* targetNode,
  const bool fromOutside)
{
  const bool cuttingTG = IsTractGroup(targetNode);
  const auto datasetNode = cuttingTG ? m_DM.GetParent(targetNode) : targetNode;
  const auto fibers =
    dynamic_cast<mitk::FilteredFiberBundle*>(datasetNode->GetData());
  const auto SO = dynamic_cast<SelectionObject*>(ROINode->GetData());

  FiberIndexes indices;
  if (!cuttingTG)
  {
    // If we are cutting on a non-filtered fibers, we ask the Filtering to
    // give us the indices passing in or near the SO.
    indices = m_Filtering.FibersToCut(
      datasetNode, fibers->GetFiberPolyData(), SO);
  }
  else
  {
    // If we are cutting on a TG, we simply use the already filtered indices
    m_Filtering.JoinAndExclude(datasetNode, targetNode, indices);
  }

  auto cutter = StreamlinesCutter(fibers, SO, fromOutside);
  const auto cuttedFibers =
    cutter.GenerateCuttedStreamlines(indices, cuttingTG);

  auto newNode = mitk::DataNode::New();
  newNode->SetName(datasetNode->GetName() + " cut");
  newNode->SetData(cuttedFibers);
  m_DM.AddNode(newNode);

  if (!cutter.DidManageColors())
  {
    m_FibersColors.SetColorsAfterCut(datasetNode, fibers, newNode);
  }
}

void FibersManager::MirrorFibers(
  mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fiber,
  const unsigned int axis)
{
  fiber->MirrorFibers(m_AnatNode->GetData()->GetGeometry(), axis);
  fiber->GetFiberPolyData()->GetPoints()->Modified();

  // We must update the colors only in ColorFromAnat mode because in all
  // other modes the colors can't change.
  if (fiber->IsInFromAnatColoring())
  {
    fiber->UpdateColorCoding(m_AnatNode);
  }

  m_Filtering.ForceUpdateForResultsLinkedTo(datasetNode);
  if (m_Filtering.UpdateResultsLinkedTo(datasetNode))
  {
    ComputeFibersVisibility(datasetNode, true);
  }
  emit DisplayStats();
}

void FibersManager::ShuffleFibers(
  mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fibers)
{
  fibers->Shuffle();

  const bool isFiltered = m_Filtering.UpdateResultsLinkedTo(datasetNode);
  if (!isFiltered)
  {
    // This call shouldn't be necessary but it's the simplest chain of calls
    // which has the responsability to update the OpenGL buffer objects, so
    // it's complex to avoid it when we are not in filtering mode.
    m_Filtering.RecalculateSubset(datasetNode);
    m_Filtering.ForceFilterDataset(datasetNode, fibers);
    m_FibersNodeData[datasetNode].UpdateIndices();
  }
  else
  {
    ComputeFibersVisibility(datasetNode, true);
    fibers->CalculateStatsUsingVisibility();
    emit DisplayStats();
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

} // namespace Fiber

} // namespace Imeka
