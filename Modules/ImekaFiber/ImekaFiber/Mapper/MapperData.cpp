#include "MapperData.hpp"

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
  vtkIdType nPts, *indices;
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
  FiberBundleFromData(newLines, newPoints, newColors, newProperties);

  return NewFiberBundle(newPolydata, newLines, newPoints, newColors);
}

void FiberMapperData::FiberBundleFromData(
  vtkCellArray* newLines,
  vtkPoints* newPoints,
  vtkUnsignedCharArray* newColors,
  vtkCellData* properties) const
{
  if (useWhole)
  {
    wholeMapperData.FiberBundleFromData(
      fiberBundle, newLines, newPoints, newColors, properties, nullptr);
    return;
  }

  QSet<const float*> alreadyDone;
  for (const auto& _partMapperData : partsMapperData)
  {
    const auto partMapperData = _partMapperData.second;
    if (partMapperData.IsEnabled())
    {
      partMapperData.FiberBundleFromData(
        fiberBundle, newLines, newPoints, newColors, properties, &alreadyDone);
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
  QSet<const float*>* alreadyDone) const
{
  vtkIdType nbPoints, *pts;
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
