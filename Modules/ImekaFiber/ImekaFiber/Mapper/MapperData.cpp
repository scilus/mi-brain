#include "MapperData.hpp"

#include <algorithm>

#include <mitkDataNode.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

namespace Imeka
{

namespace Fiber
{

PartMapperData::PartMapperData()
  : m_NbPoints(0)
  , m_Enabled(true)
{}

void FiberMapperData::SetIndices(
  const mitk::DataNode* TGNode,
  const FiberIndexes& visibility)
{
  unsigned int totalNbPoints = 0;
  auto polyData = fiberBundle->GetFiberPolyData();
  vtkIdType nPts;
  const vtkIdType *indices;
  for (const auto cellID : visibility)
  {
    polyData->GetCellPoints(cellID, nPts, indices);
    for (int i = 0; i < nPts - 1; ++i)
    {
      m_IBO.push_back(static_cast<unsigned int>(indices[i]));
      m_IBO.push_back(static_cast<unsigned int>(indices[i + 1]));
    }
    totalNbPoints += nPts;
  }

  if (TGNode)
  {
    auto& tgMapperData = partsMapperData[TGNode];
    tgMapperData.SetIndices(visibility);
    tgMapperData.SetNbPoints(totalNbPoints);
    tgMapperData.Enable();
    useWhole = false;
  }
  else
  {
    wholeMapperData.SetIndices(visibility);
    wholeMapperData.SetNbPoints(totalNbPoints);
    wholeMapperData.Enable();
    useWhole = true;
  }
}

unsigned int FiberMapperData::TotalNbPoints() const
{
  if (useWhole) { return wholeMapperData.NbPoints(); }

  unsigned int totalNbPoints = 0;
  for (const auto& _partMapperData : partsMapperData)
  {
    const auto partMapperData = _partMapperData.second;
    totalNbPoints += partMapperData.NbPoints();
  }

  return totalNbPoints;
}

mitk::FilteredFiberBundle::Pointer FiberMapperData::FiberBundleFromData(
  const bool keepColors, const bool keepProperties) const
{
  auto newLines = vtkSmartPointer<vtkCellArray>::New();
  auto newPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkUnsignedCharArray> newColors = nullptr;
  if (keepColors || !fiberBundle->IsInOrientationColoring())
  {
    newColors = mitk::FilteredFiberBundle::GetNewColorArray(TotalNbPoints());
  }

  auto newPolydata = vtkSmartPointer<vtkPolyData>::New();
  vtkCellData *newProperties = nullptr;
  if (keepProperties)
  {
    CopyPropertiesSettings(fiberBundle->GetFiberPolyData(), newPolydata);
    newProperties = newPolydata->GetCellData();
  }
  mitk::FilteredFiberBundle::StreamlineGroups groups;
  FiberBundleFromData(newLines, newPoints, newColors, newProperties, &groups);

  auto newBundle = NewFiberBundle(newPolydata, newLines, newPoints, newColors);
  newBundle->SetStreamlineGroups(groups);
  return newBundle;
}

namespace
{
  // Indices of the written streamlines, in the new fiber bundle
  template <typename Container>
  std::vector<vtkIdType> MapIndices(
    const QHash<vtkIdType, vtkIdType>& newIds,
    const Container& oldIndices)
  {
    std::vector<vtkIdType> indices;
    for (const auto oldIdx : oldIndices)
    {
      const auto it = newIds.constFind(oldIdx);
      if (it != newIds.constEnd()) { indices.push_back(it.value()); }
    }
    return indices;
  }
}

void FiberMapperData::FiberBundleFromData(
  vtkCellArray* newLines,
  vtkPoints* newPoints,
  vtkUnsignedCharArray* newColors,
  vtkCellData* properties,
  mitk::FilteredFiberBundle::StreamlineGroups* groups) const
{
  QHash<vtkIdType, vtkIdType> newIds;
  auto newIdsPtr = groups ? &newIds : nullptr;

  if (useWhole)
  {
    wholeMapperData.FiberBundleFromData(fiberBundle, newLines, newPoints,
      newColors, properties, nullptr, newIdsPtr);

    // No tract group is active, so keep the groups of the file, restricted to
    // the written streamlines.
    if (groups)
    {
      for (const auto& name_indexes : fiberBundle->GetStreamlineGroups())
      {
        AddStreamlineGroup(*groups, name_indexes.first,
          MapIndices(newIds, name_indexes.second));
      }
    }
    return;
  }

  QSet<const float*> alreadyDone;
  for (const auto& _partMapperData : partsMapperData)
  {
    const auto& partMapperData = _partMapperData.second;
    if (partMapperData.IsEnabled())
    {
      partMapperData.FiberBundleFromData(fiberBundle, newLines, newPoints,
        newColors, properties, &alreadyDone, newIdsPtr);
    }
  }

  // A streamline can be in many tract groups, but it's written only once, so
  // the groups are computed once everything is written.
  if (groups)
  {
    for (const auto& TGNode_partMapperData : partsMapperData)
    {
      const auto TGNode = TGNode_partMapperData.first;
      const auto& partMapperData = TGNode_partMapperData.second;
      // Hidden TRX groups can be enabled with stale indices: their node
      // visibility is their only reliable state.
      if (!partMapperData.IsEnabled() || !TGNode->IsVisible(nullptr))
      {
        continue;
      }
      AddStreamlineGroup(*groups, TGNode->GetName(),
        MapIndices(newIds, partMapperData.GetIndices()));
    }
  }
}

mitk::FilteredFiberBundle::Pointer PartMapperData::FiberBundleFromData(
  mitk::FilteredFiberBundle* rawFibers,
  const bool keepColors,
  const bool keepProperties) const
{
  if (m_NbPoints == 0) { return nullptr; }

  auto newLines = vtkSmartPointer<vtkCellArray>::New();
  auto newPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkUnsignedCharArray> newColors = nullptr;
  if (keepColors)
  {
    newColors = mitk::FilteredFiberBundle::GetNewColorArray(m_NbPoints);
  }

  auto newPolydata = vtkSmartPointer<vtkPolyData>::New();
  vtkCellData *newProperties = nullptr;
  if (keepProperties)
  {
    CopyPropertiesSettings(rawFibers->GetFiberPolyData(), newPolydata);
    newProperties = newPolydata->GetCellData();
  }

  FiberBundleFromData(
    rawFibers, newLines, newPoints, newColors, newProperties, nullptr);

  return NewFiberBundle(newPolydata, newLines, newPoints, newColors);
}

void PartMapperData::FiberBundleFromData(
  mitk::FilteredFiberBundle* rawFibers,
  vtkCellArray* newLines,
  vtkPoints* newPoints,
  vtkUnsignedCharArray* newColors,
  vtkCellData* newProperties,
  QSet<const float*>* alreadyDone,
  QHash<vtkIdType, vtkIdType>* newIds) const
{
  vtkIdType nbPoints;
  const vtkIdType *pts;
  auto oldPolyData = rawFibers->GetFiberPolyData();
  auto points = static_cast<float*>(oldPolyData->GetPoints()->GetVoidPointer(0));
  auto colors = static_cast<unsigned char*>(
    rawFibers->GetFiberColors()->GetVoidPointer(0));
  PropertiesHelperToWriteTrk propertiesHelper(
    oldPolyData->GetCellData(), newProperties);

  for (auto cellID : m_Indices)
  {
    oldPolyData->GetCellPoints(cellID, nbPoints, pts);
    auto streamlinePoints = points + 3 * *pts;
    auto streamlineColors = colors + 4 * *pts;

    if (alreadyDone)
    {
      if (alreadyDone->contains(streamlinePoints)) { continue; }
      alreadyDone->insert(streamlinePoints);
    }

    if (newIds)
    {
      newIds->insert(cellID, newLines->GetNumberOfCells());
    }
    newLines->InsertNextCell(nbPoints);
    for (vtkIdType pointID = 0; pointID < nbPoints; ++pointID)
    {
      newLines->InsertCellPoint(newPoints->InsertNextPoint(streamlinePoints));
      streamlinePoints += 3;
      if (newColors)
      {
        newColors->InsertNextTypedTuple(streamlineColors);
        streamlineColors += 4;
      }
    }

    if (newProperties)
    {
      propertiesHelper.Save(cellID);
    }
  }
}

void AddStreamlineGroup(
  mitk::FilteredFiberBundle::StreamlineGroups& groups,
  const std::string& name,
  std::vector<vtkIdType> indices)
{
  if (indices.empty()) { return; }

  std::sort(indices.begin(), indices.end());
  auto uniqueName = name;
  for (int i = 2; groups.count(uniqueName); ++i)
  {
    uniqueName = name + " (" + std::to_string(i) + ")";
  }
  groups[uniqueName] = std::move(indices);
}

mitk::FilteredFiberBundle::Pointer NewFiberBundle(
  vtkSmartPointer<vtkPolyData> polydata,
  vtkCellArray* lines,
  vtkPoints* points,
  vtkUnsignedCharArray* colors)
{
  polydata->SetPoints(points);
  polydata->SetLines(lines);
  auto newBundle = mitk::FilteredFiberBundle::New(polydata);
  if (colors)
  {
    newBundle->SetFiberColors(colors);
  }
  return newBundle;
}

} // namespace Fiber

} // namespace Imeka
