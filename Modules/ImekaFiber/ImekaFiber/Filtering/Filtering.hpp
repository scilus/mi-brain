#ifndef IMEKA_FIBER_FILTERING_HPP_INCLUDED
#define IMEKA_FIBER_FILTERING_HPP_INCLUDED

#include <random>

#include "ImekaFiber/Filtering/Result.hpp"
#include "ImekaFiber/Filtering/SOFiltering.hpp"
#include "ImekaFiber/Filtering/SurfaceFiltering.hpp"
#include "ImekaFiber/FiberNodeData.hpp"

#include "ImekaFiberExports.h"

namespace mitk { class FilteredFiberBundle; }

namespace Imeka
{

namespace Fiber
{

struct DataSetInfo {
  const mitk::FilteredFiberBundle* fiber;

  // Sent to m_SOFiltering and m_SurfaceFiltering when filtering
  DatasetFilteringData data;

  // TractGroupNode -> TractGroup
  // TractGroup is sent to ActiveROIsDialogBox, where it's directly modified
  std::map<const mitk::DataNode*, TractGroup> tractGroups;

  // ROI -> Results
  // We update the raw results as rarely as possible. Ww simply do set
  // operations on them when the fibers need to be updated.
  std::map<const mitk::BaseData*, Result> results;
};

/*
Public class for SO and Surface filtering.

Keeps the results in memory. Knows when to recalculate the results.
*/
class ImekaFiber_EXPORT Filtering
{
public:
  Filtering(Imeka::Fiber::NodeDataMap&);

  void WaitForAllFutures() const;

  void SetAnat(const mitk::Image*);

  std::map<const mitk::DataNode*, DataSetInfo>& DatasetInfo() { return m_DatasetInfo; }
  void RecalculateSubset(const mitk::DataNode*);

  void AddDataset(const mitk::DataNode*, mitk::FilteredFiberBundle*);
  void UpdateDataset(const mitk::DataNode*, mitk::FilteredFiberBundle*);
  TractGroup& DuplicateDataset(
    const mitk::DataNode*, mitk::DataNode*, mitk::DataNode*);
  void DeleteDataset(const mitk::DataNode*);

  // After a Mirror Fibers, we want the next call to `UpdateResultsLinkedTo`
  // to do something, we this call resets the `lastUpdate` of all results
  // linked to this `datasetNode`.
  void ForceUpdateForResultsLinkedTo(const mitk::DataNode*);
  bool UpdateResultsLinkedTo(const mitk::DataNode*);
  bool UpdateResults(const mitk::BaseData*, const bool);
  bool UpdateResults(
    const mitk::BaseData*, const mitk::DataNode*, DataSetInfo&, const bool);

  void EnableROIInTractGroup(
    const mitk::DataNode*, const mitk::DataNode*, const mitk::BaseData*);
  void DisableROIInTractGroup(
    const mitk::DataNode*, mitk::DataNode*, const mitk::BaseData*);
  const mitk::DataNode* DeleteROIInTractGroup(
    const mitk::DataNode*, const mitk::DataNode*, const mitk::BaseData*);

  Nodes ReplaceROI(const mitk::Surface*, const mitk::Surface*);
  ConstNodes DeleteROI(const mitk::BaseData*);

  void EnableTractGroup(const mitk::DataNode*, const mitk::DataNode*);
  void DisableTractGroup(const mitk::DataNode*, mitk::DataNode*);
  void DeleteTractGroup(const mitk::DataNode*, const mitk::DataNode*);
  TractGroup& GetTractGroup(const mitk::DataNode*, const mitk::DataNode*);
  void UpdateTractGroup(const mitk::DataNode*, TractGroup&);

  void SetMinLength(const float);
  void SetMaxLength(const float);
  void SetLengths(const float, const float);
  void SetSkipParameters(
    const float, const unsigned int, const bool);

  std::set<const mitk::BaseData*> LinkedROIs(const mitk::DataNode*) const;
  Nodes LinkedDatasets(const mitk::BaseData*) const;

  void ForceFilterDataset(const mitk::DataNode*, mitk::FilteredFiberBundle*);
  bool FilterDataset(
    const mitk::DataNode*, mitk::FilteredFiberBundle*, const bool);
  void RealFiltering(
    DataSetInfo&, const mitk::BaseData*, Imeka::Fiber::Result&, const bool);
  bool JoinAndExclude(
    const mitk::DataNode*, const mitk::DataNode*, FiberIndexes&) const;
  FiberIndexes FibersToCut(
    const mitk::DataNode*, vtkPolyData*, const SelectionObject*) const;

private:
  void FillDatasetInfo(
    const mitk::DataNode*, const mitk::FilteredFiberBundle*);
  void RecalculateSubsetOfAllDatasets();
  FiberIndexes SubsetOf(const mitk::FilteredFiberBundle*);

  bool HasAnyActiveTractGroup() const;
  bool HasAnyActiveROI(const mitk::DataNode*, const mitk::DataNode*) const;
  bool IsInAnyActiveTractGroup(const mitk::BaseData*) const;
  bool IsInAnyActiveTractGroup(
    const mitk::BaseData*, const DataSetInfo&) const;
  bool IsInAnyActiveTractGroup(
    const mitk::BaseData*, const DataSetInfo&, const mitk::DataNode*) const;
  bool IsInAnyTractGroup(const mitk::BaseData*, const DataSetInfo&) const;
  bool IsInAnyTractGroup(
    const mitk::BaseData*, const DataSetInfo&, const mitk::DataNode*) const;

  Imeka::Fiber::NodeDataMap& m_NodeDataMap;
  Imeka::Fiber::SOFiltering m_SOFiltering;
  Imeka::Fiber::SurfaceFiltering m_SurfaceFiltering;
  std::mt19937 m_Rng;

  // DatasetNode -> ROI -> Results
  // DatasetNode -> TractGroupNode -> TractGroup -> Lines
  std::map<const mitk::DataNode*, DataSetInfo> m_DatasetInfo;

  float m_MinLength, m_MaxLength;
  float m_TractGroupTo;
  unsigned int m_UpTo;

  mutable const mitk::BaseData* m_LastMovedSO;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FILTERING_HPP_INCLUDED
