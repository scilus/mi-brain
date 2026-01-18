#include "Filtering.hpp"

#include <QtConcurrentRun>
#include <limits>

#include "common.hpp"

namespace Imeka
{

namespace Fiber
{

// Called in another thread to get a QFuture. This way, the user doesn't have
// to wait for this results. It's useless for him anyway.
vtkSmartPointer<vtkOctreePointLocator> GetReadyOctree(
  const mitk::FilteredFiberBundle* fiber)
{
  auto octree = vtkSmartPointer<vtkOctreePointLocator>::New();
  octree->SetDataSet(fiber->GetFiberPolyData());
  octree->BuildLocator();
  return octree;
}

Filtering::Filtering(Imeka::Fiber::NodeDataMap& ndm)
  : m_NodeDataMap(ndm)
  , m_LastMovedSO(nullptr)
  , m_MinLength(0.0f)
  , m_MaxLength(std::numeric_limits<float>::max())
{
  m_Rng.seed(std::random_device()());
}

void Filtering::WaitForAllFutures() const
{
  for (const auto& p : m_DatasetInfo)
  {
    // Yes, volatile, because I don't want it to be optimized away.
    volatile auto result = p.second.data.octreeFuture.result();
  }
}

void Filtering::SetAnat(const mitk::Image* anat)
{
  //m_SOFiltering.SetAnat(anat);
  m_SurfaceFiltering.SetAnat(anat);
}

void Filtering::AddDataset(
  const mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fiber)
{
  FillDatasetInfo(datasetNode, fiber);
}

void Filtering::UpdateDataset(
  const mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fiber)
{
  auto datasetInfo = m_DatasetInfo[datasetNode];
  FillDatasetInfo(datasetNode, fiber);

  // The dataset has been updated and the tractGroups are still valid,
  // but the results aren't
  for (auto& roi_result : datasetInfo.results)
  {
    const auto ROI = roi_result.first;
    auto& results = roi_result.second;
    m_SOFiltering.Update(datasetInfo.data, ROI, results, true);
    m_SurfaceFiltering.Update(
      datasetInfo.fiber, datasetInfo.data, ROI, results);
  }
}

TractGroup& Filtering::DuplicateDataset(
  const mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode,
  mitk::DataNode* newTGNode)
{
  auto& datasetInfo = m_DatasetInfo[datasetNode];
  return datasetInfo.tractGroups[newTGNode] = datasetInfo.tractGroups[TGNode];
}

void Filtering::FillDatasetInfo(
  const mitk::DataNode* datasetNode,
  const mitk::FilteredFiberBundle* fiber)
{
  const bool alreadyIn = m_DatasetInfo.count(datasetNode) > 0;
  auto& datasetInfo = m_DatasetInfo[datasetNode];

  double additionalBoundingMargin = datasetInfo.data.additionalBoundingMargin;
  if (!alreadyIn)
  {
    const auto fibersProperties =
      Imeka::Fiber::ComputeSegmentProperties(fiber->GetFiberPolyData());
    additionalBoundingMargin = fibersProperties.second / 2.0;
  }

  datasetInfo.fiber = fiber;
  datasetInfo.data = {
    SubsetOf(fiber),
    QtConcurrent::run(GetReadyOctree, fiber),
    additionalBoundingMargin
  };
}

void Filtering::DeleteDataset(const mitk::DataNode* datasetNode)
{
  auto it = m_DatasetInfo.find(datasetNode);
  if (it != m_DatasetInfo.end())
  {
    // We MIGHT have to "destroy" the QFuture here, but:
    // - We can't destroy or cancel a QFuture started with QtConcurrent::run.
    //   We must wait for it to finish normally.
    // - I *think* it's not actually important to wait for it. The DataNode and
    //   BaseData will be destroyed and their memory freed. The vtkPolyData
    //   will be destroyed and freed only when the QFuture finishes.
    // - If the program closes, in will wait for the QFutures to end.
    m_DatasetInfo.erase(it);
  }
}

void Filtering::EnableROIInTractGroup(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode,
  const mitk::BaseData* ROI)
{
  auto& datasetInfo = m_DatasetInfo[datasetNode];
  datasetInfo.tractGroups[TGNode].lines[ROI].enabled = true;

  if (!datasetInfo.results[ROI].IsUpToDate(ROI))
  {
    UpdateResults(ROI, datasetNode, datasetInfo, true);
  }
  m_NodeDataMap[datasetNode].fiberMapperData.partsMapperData[TGNode].Enable();
}

void Filtering::DisableROIInTractGroup(
  const mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode,
  const mitk::BaseData* ROI)
{
  auto& lines = m_DatasetInfo[datasetNode].tractGroups[TGNode].lines;
  lines[ROI].enabled = false;

  if (!HasAnyActiveROI(datasetNode, TGNode))
  {
    TGNode->SetVisibility(false);
    m_NodeDataMap[datasetNode].fiberMapperData
      .partsMapperData[TGNode].Disable();
  }
}

const mitk::DataNode* Filtering::DeleteROIInTractGroup(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode,
  const mitk::BaseData* ROI)
{
  auto& lines = m_DatasetInfo[datasetNode].tractGroups[TGNode].lines;
  auto it = lines.find(ROI);
  const auto linkedActiveROI = it->second.activeROINode;
  if (lines.size() == 1)
  {
    DeleteTractGroup(datasetNode, TGNode);
  }
  else
  {
    lines.erase(it);
  }
  return linkedActiveROI;
}

void Filtering::ForceUpdateForResultsLinkedTo(
  const mitk::DataNode* datasetNode)
{
  for (auto ROI : LinkedROIs(datasetNode))
  {
    for (auto& node_info : m_DatasetInfo)
    {
      const auto datasetNode = node_info.first;
      DataSetInfo& datasetInfo = node_info.second;
      if (datasetInfo.results.count(ROI) == 0) { break; }
      if (datasetNode->IsVisible(nullptr) && IsInAnyActiveTractGroup(ROI, datasetInfo))
      {
        datasetInfo.results[ROI].lastUpdate = 0;
      }
    }
  }
}

bool Filtering::UpdateResultsLinkedTo(const mitk::DataNode* datasetNode)
{
  bool wasUpdated = false;
  for (auto ROI : LinkedROIs(datasetNode))
  {
    wasUpdated |= UpdateResults(ROI, true);
  }
  return wasUpdated;
}

bool Filtering::UpdateResults(
  const mitk::BaseData* ROI,
  const bool precise)
{
  bool wasUpdated = false;
  for (auto& node_info : m_DatasetInfo)
  {
    const auto datasetNode = node_info.first;
    DataSetInfo& datasetInfo = node_info.second;
    if (datasetInfo.results.count(ROI) == 0 || !datasetInfo.results[ROI].IsUpToDate(ROI))
    {
      wasUpdated |= UpdateResults(ROI, datasetNode, datasetInfo, precise);
    }
  }
  return wasUpdated;
}

bool Filtering::UpdateResults(
  const mitk::BaseData* ROI,
  const mitk::DataNode* datasetNode,
  DataSetInfo& datasetInfo,
  const bool precise)
{
  bool wasUpdated = false;
  if (datasetNode->IsVisible(nullptr) && IsInAnyActiveTractGroup(ROI, datasetInfo))
  {
    m_LastMovedSO = ROI;
    RealFiltering(datasetInfo, ROI, datasetInfo.results[ROI], precise);
    wasUpdated = true;
  }
  return wasUpdated;
}

Nodes Filtering::ReplaceROI(
  const mitk::Surface* oldSurface,
  const mitk::Surface* newSurface)
{
  Nodes linkedDatasets;
  for (auto& node_info : m_DatasetInfo)
  {
    auto& datasetInfo = node_info.second;

    // Can't use IsInAnyActiveTractGroup because we're also erasing data
    bool active = false;
    for (auto& TGNode_TG : datasetInfo.tractGroups)
    {
      auto& lines = TGNode_TG.second.lines;
      const auto it = lines.find(oldSurface);
      if (it != lines.end())
      {
        lines[newSurface] = it->second;
        lines.erase(it);
        active = true;
      }
    }
    if (!active) { continue; }

    auto& results = datasetInfo.results;
    results.erase(results.find(oldSurface));

    RealFiltering(datasetInfo, newSurface, results[newSurface], true);
    linkedDatasets.push_back(
      const_cast<mitk::DataNode*>(node_info.first));
  }
  return linkedDatasets;
}

ConstNodes Filtering::DeleteROI(const mitk::BaseData* ROI)
{
  ConstNodes concernedActiveROIs;
  for (auto& node_info : m_DatasetInfo)
  {
    auto& datasetInfo = node_info.second;
    if (!IsInAnyTractGroup(ROI, datasetInfo)) { continue; }

    // Can't modify datasetInfo.tractGroups while iterating on it
    const auto tractGroupsCopy = datasetInfo.tractGroups;
    for (auto& TGNode_TG : tractGroupsCopy)
    {
      const auto TGNode = TGNode_TG.first;
      if (IsInAnyTractGroup(ROI, datasetInfo, TGNode))
      {
        concernedActiveROIs.push_back(
          DeleteROIInTractGroup(node_info.first, TGNode, ROI));
      }
    }

    auto& resultsMap = datasetInfo.results;
    auto it = resultsMap.find(ROI);
    if (it != resultsMap.end()) { resultsMap.erase(it); }
  }
  return concernedActiveROIs;
}

void Filtering::EnableTractGroup(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode)
{
  for (auto& p : GetTractGroup(datasetNode, TGNode).lines)
  {
    EnableROIInTractGroup(datasetNode, TGNode, p.first);
  }
}

void Filtering::DisableTractGroup(
  const mitk::DataNode* datasetNode,
  mitk::DataNode* TGNode)
{
  for (auto& p : GetTractGroup(datasetNode, TGNode).lines)
  {
    DisableROIInTractGroup(datasetNode, TGNode, p.first);
  }
}

void Filtering::DeleteTractGroup(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode)
{
  auto& tractGroups = m_DatasetInfo[datasetNode].tractGroups;
  tractGroups.erase(tractGroups.find(TGNode));

  // There can be no mapperData if TG is deleted as soon as it's created.
  auto& pmd = m_NodeDataMap[datasetNode].fiberMapperData.partsMapperData;
  if (!pmd.empty())
  {
    auto it = pmd.find(TGNode);
    if (it != pmd.end()) { pmd.erase(it); }
  }
}

TractGroup& Filtering::GetTractGroup(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode)
{
  return m_DatasetInfo[datasetNode].tractGroups[TGNode];
}

void Filtering::UpdateTractGroup(
  const mitk::DataNode* datasetNode,
  TractGroup& TG)
{
  if (!TG.modified) { return; }

  auto& datasetInfo = m_DatasetInfo[datasetNode];
  auto& resultsMap = datasetInfo.results;
  for (const auto& p : TG.lines)
  {
    const auto ROIData = p.first;
    const auto line = p.second;

    if (line.enabled)
    {
      // Is this ROI already calculated for this dataset? If yes, then we do
      // nothing because the results are updated only when the BaseData
      // changes, not when we play with the tractGroups
      if (resultsMap.find(ROIData) == resultsMap.end()
        || !datasetInfo.results[ROIData].IsUpToDate(ROIData))
      {
        auto& results = resultsMap[ROIData]; // Create it or get it
        RealFiltering(datasetInfo, ROIData, results, true);
      }
    }
  }

  TG.modified = false;
}

std::set<const mitk::BaseData*> Filtering::LinkedROIs(
  const mitk::DataNode* datasetNode) const
{
  std::set<const mitk::BaseData*> linkedROIs;

  const DataSetInfo& datasetInfo = m_DatasetInfo.at(datasetNode);
  for (auto& ROI_results_pair : datasetInfo.results)
  {
    auto ROI = ROI_results_pair.first;
    if (IsInAnyActiveTractGroup(ROI, datasetInfo))
    {
      linkedROIs.insert(ROI);
    }
  }

  return linkedROIs;
}

Nodes Filtering::LinkedDatasets(const mitk::BaseData* ROI) const
{
  Nodes linkedDatasets;
  for (auto& node_info : m_DatasetInfo)
  {
    const DataSetInfo& datasetInfo = node_info.second;
    if (IsInAnyActiveTractGroup(ROI, datasetInfo))
    {
      linkedDatasets.push_back(
        const_cast<mitk::DataNode*>(node_info.first));
    }
  }

  return linkedDatasets;
}

bool Filtering::HasAnyActiveTractGroup() const
{
  for (auto& node_info : m_DatasetInfo)
  {
    for (auto& TGNode_TG : node_info.second.tractGroups)
    {
      if (HasAnyActiveROI(node_info.first, TGNode_TG.first))
      {
        return true;
      }
    }
  }
  return false;
}

bool Filtering::HasAnyActiveROI(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode) const
{
  const auto& tractGroups = m_DatasetInfo.at(datasetNode).tractGroups;
  for (const auto& roi_lines : tractGroups.at(TGNode).lines)
  {
    if (roi_lines.second.enabled)
    {
      return true;
    }
  }
  return false;
}

bool Filtering::IsInAnyActiveTractGroup(const mitk::BaseData* ROI) const
{
  for (const auto& node_info : m_DatasetInfo)
  {
    if (IsInAnyActiveTractGroup(ROI, node_info.second))
    {
      return true;
    }
  }

  return false;
}

bool Filtering::IsInAnyActiveTractGroup(
  const mitk::BaseData* ROI,
  const DataSetInfo& datasetInfo) const
{
  for (auto& TGNode_TG : datasetInfo.tractGroups)
  {
    if (IsInAnyActiveTractGroup(ROI, datasetInfo, TGNode_TG.first))
    {
      return true;
    }
  }
  return false;
}

bool Filtering::IsInAnyActiveTractGroup(
  const mitk::BaseData* ROI,
  const DataSetInfo& datasetInfo,
  const mitk::DataNode* TGNode) const
{
  auto& lines = datasetInfo.tractGroups.at(TGNode).lines;
  const auto it = lines.find(ROI);
  return it != lines.end() && it->second.enabled;
}

bool Filtering::IsInAnyTractGroup(
  const mitk::BaseData* ROI,
  const DataSetInfo& datasetInfo) const
{
  for (auto& TGNode_TG : datasetInfo.tractGroups)
  {
    if (IsInAnyTractGroup(ROI, datasetInfo, TGNode_TG.first))
    {
      return true;
    }
  }
  return false;
}

bool Filtering::IsInAnyTractGroup(
  const mitk::BaseData* ROI,
  const DataSetInfo& datasetInfo,
  const mitk::DataNode* TGNode) const
{
  auto& lines = datasetInfo.tractGroups.at(TGNode).lines;
  const auto it = lines.find(ROI);
  return it != lines.end();
}

FiberIndexes Filtering::FibersToCut(
  const mitk::DataNode* datasetNode,
  vtkPolyData* fiberPolyData,
  const SelectionObject* SO) const
{
  // Are the results already calculated for this SO in this dataset?
  const auto& datasetInfo = m_DatasetInfo.at(datasetNode);
  const auto& results = datasetInfo.results;
  const auto it = results.find(SO);
  if (it != results.end() && it->second.IsUpToDate(SO))
  {
    return it->second.anyPart;
  }

  auto octree = datasetInfo.data.octreeFuture.result();
  return m_SOFiltering.FibersTouchingTheBox(fiberPolyData, octree, SO);
}

bool Filtering::FilterDataset(
  const mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fibers,
  const bool precise)
{
  bool atLeastOneActiveROI = false;
  bool wasUpdated = false;
  auto& fiberMapperData = m_NodeDataMap[datasetNode].fiberMapperData;
  auto& datasetInfo = m_DatasetInfo.at(datasetNode);
  const auto& generalVisibility = datasetInfo.data.visibleTractGroupIndexes;
  
  FiberIndexes unitedSelection;;
  for (auto TGNode_ : datasetInfo.tractGroups)
  {
    const auto TGNode = TGNode_.first;

    FiberIndexes filteredVisibility;
    atLeastOneActiveROI |= JoinAndExclude(
      datasetNode, TGNode, filteredVisibility);
    if (atLeastOneActiveROI)
    {
      // We must clear the IBO only if there was at least one active ROI and we
      // must clear it before adding anything, so it must be done here.
      if (!wasUpdated)
      {
        fiberMapperData.ClearIndices();
      }
      fiberMapperData.SetIndices(TGNode, generalVisibility & filteredVisibility);
      wasUpdated = true;

      if (precise)
      {
        unitedSelection |= filteredVisibility;
      }
    }
  }

  m_LastMovedSO = nullptr;

  // There are 3 possibilities in this method:
  // - Filtering mode + min/max length if there's at least one active ROI
  // - Filter only by min/max length if there was a change
  // - Do nothing
  if (atLeastOneActiveROI)
  {
    if (precise)
    {
      const auto visibility = generalVisibility & unitedSelection;
      fibers->SetFibersVisibility(visibility);
    }
  }
  // If there is no active ROI in any tractGroup, we display all fibers
  else if (fibers->GetFibersVisibility() != generalVisibility)
  {
    if (precise)
    {
      fibers->SetFibersVisibility(generalVisibility);
    }
    fiberMapperData.ClearIndices();
    fiberMapperData.SetIndices(nullptr, generalVisibility);
    wasUpdated = true;
  }

  return wasUpdated;
}

// We set the current `visibleTractGroupIndexes` as the new indices, which
// don't seem to make much sense, but 1) it may have been updated before 2)
// the streamlines may have been shuffled, or mirrored, etc.
void Filtering::ForceFilterDataset(
  const mitk::DataNode* datasetNode,
  mitk::FilteredFiberBundle* fibers)
{
  auto& fiberMapperData = m_NodeDataMap[datasetNode].fiberMapperData;
  auto& datasetInfo = m_DatasetInfo.at(datasetNode);
  const auto& generalVisibility = datasetInfo.data.visibleTractGroupIndexes;
  fiberMapperData.ClearIndices();
  fiberMapperData.SetIndices(nullptr, generalVisibility);
}

bool Filtering::JoinAndExclude(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode,
  FiberIndexes& unitedSelection) const
{
  // If there is only one NOT, an additional operation is
  // needed before the NOT is applied.
  FiberIndexes AND, NOT;
  bool atLeastOneAND = false;
  bool atLeastOneROIActiveInTractGroup = false;

  const auto& datasetInfo = m_DatasetInfo.at(datasetNode);
  const auto& TG = datasetInfo.tractGroups.at(TGNode);
  const auto& linesMap = TG.lines;

  // We do not want to lose time filtering anything if the modified ROI
  // isn't even used in this dataset
  if (m_LastMovedSO && !IsInAnyActiveTractGroup(m_LastMovedSO, datasetInfo))
  { return false; }

  for (const auto& p : linesMap)
  {
    const auto ROIData = p.first;
    const auto line = p.second;
    if (!line.enabled) { continue; }

    atLeastOneROIActiveInTractGroup = true;
    const auto& results = datasetInfo.results.at(ROIData);
    const auto selectedStreamlines = results.GetSelectedIndexes(line.mode);

    if (line.isNot)
    {
      NOT.reserve(selectedStreamlines.size());
      NOT = NOT.unite(selectedStreamlines);
    }
    else if (AND.size() == 0 && !atLeastOneAND)
    {
      // The first AND selects the streamlines
      AND.reserve(selectedStreamlines.size());
      AND = AND.unite(selectedStreamlines);
      atLeastOneAND = true;
    }
    else
    {
      // The subsequent ANDs are intersected with the inital AND in order to
      // only keep the fibers selected by all ANDs.
      AND = AND.intersect(selectedStreamlines);
    }
  }

  if (!atLeastOneAND && atLeastOneROIActiveInTractGroup)
  {
    // If there is no AND and at least one NOT, the exclusion will be
    // performed on the complete dataset (all the streamlines).
    const vtkIdType nbFibers = datasetInfo.fiber->GetNumFibers();
    AND.reserve(nbFibers);
    for (vtkIdType j = 0; j < nbFibers; ++j)
    {
      AND.insert(j);
    }
  }

  // The last step is to remove the NOT tractGroup from the final AND
  // tractGroup (intersection of all the AND tractGroup).
  for (const auto idx : AND)
  {
    if (!NOT.contains(idx))
    {
      unitedSelection.insert(idx);
    }
  }
  return atLeastOneROIActiveInTractGroup;
}

void Filtering::RealFiltering(
  DataSetInfo& datasetInfo,
  const mitk::BaseData* ROI,
  Imeka::Fiber::Result& results,
  const bool precise)
{
  m_SOFiltering.Update(datasetInfo.data, ROI, results, precise);
  m_SurfaceFiltering.Update(
    datasetInfo.fiber, datasetInfo.data, ROI, results);
  results.lastUpdate = ROI->GetMTime();
}

void Filtering::SetMinLength(const float f)
{
  m_MinLength = f;
  RecalculateSubsetOfAllDatasets();
}

void Filtering::SetMaxLength(const float f)
{
  m_MaxLength = f;
  RecalculateSubsetOfAllDatasets();
}

void Filtering::SetLengths(const float minLength, const float maxLength)
{
  m_MinLength = minLength;
  m_MaxLength = maxLength;
  RecalculateSubsetOfAllDatasets();
}

void Filtering::SetSkipParameters(
  const float to, const unsigned int upTo, const bool showAll)
{
  if (showAll)
  {
    m_TractGroupTo = 1.0;
    m_UpTo = std::numeric_limits<unsigned int>::max();
  }
  else
  {
    m_TractGroupTo = to;
    m_UpTo = upTo;
  }
  RecalculateSubsetOfAllDatasets();
}

void Filtering::RecalculateSubsetOfAllDatasets()
{
  for (auto& node_info : m_DatasetInfo)
  {
    RecalculateSubset(node_info.first);
  }
}

void Filtering::RecalculateSubset(const mitk::DataNode* datasetNode)
{
  auto& datasetInfo = m_DatasetInfo[datasetNode];
  datasetInfo.data.visibleTractGroupIndexes = SubsetOf(datasetInfo.fiber);
}

FiberIndexes Filtering::SubsetOf(const mitk::FilteredFiberBundle* fiber)
{
  FiberIndexes visible;

  auto Add = [this, fiber, &visible](
    const unsigned int fromIdx,
    const unsigned int toIdx)
  {
    const auto& fiberLengths = fiber->GetFiberLengths();
    for (vtkIdType i = fromIdx; i < toIdx; ++i)
    {
      const float length = fiberLengths[i];
      if (length >= m_MinLength && length <= m_MaxLength)
      {
        visible.insert(i);
      }
    }
  };

  const unsigned int nbFibers = fiber->GetFiberPolyData()->GetNumberOfCells();
  if (m_TractGroupTo == 1.0 && m_UpTo >= nbFibers)
  {
    Add(0, nbFibers);
  }
  else
  {
    const unsigned int nbToShow = std::min(
      m_UpTo, static_cast<unsigned int>(m_TractGroupTo * nbFibers));

    std::uniform_int_distribution<std::mt19937::result_type>
      getRandomInt(0, nbFibers);
    const unsigned int fromIdx = getRandomInt(m_Rng);
    if (fromIdx + nbToShow >= nbFibers)
    {
      Add(fromIdx, nbFibers);
      Add(0, nbToShow - (nbFibers - fromIdx));
    }
    else
    {
      Add(fromIdx, fromIdx + nbToShow);
    }
  }

  return visible;
}

} // namespace Fiber

} // namespace Imeka
