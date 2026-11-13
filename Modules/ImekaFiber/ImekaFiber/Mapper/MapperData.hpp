#ifndef IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED
#define IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED

#include <QHash>

#include "FiberBundle/TrkProperties.hpp"
#include "../types.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT PartMapperData
{
public:
  PartMapperData();

  mitk::FilteredFiberBundle::Pointer FiberBundleFromData(
    mitk::FilteredFiberBundle*, const bool, const bool) const;

  void SetIndices(const FiberIndexes& indices) { m_Indices = indices; }
  const FiberIndexes& GetIndices() const { return m_Indices; }
  void SetNbPoints(const unsigned int nbPoints) { m_NbPoints = nbPoints; }
  unsigned int NbPoints() const { return m_NbPoints; }
  void Enable() { m_Enabled = true; }
  void Disable() { m_Enabled = false; }
  bool IsEnabled() const { return m_Enabled; }

private:
  friend struct FiberMapperData;
  // `newIds`, if given, receives the index of each written streamline in
  // `newLines` (key: streamline index in the source fiber bundle).
  void FiberBundleFromData(
    mitk::FilteredFiberBundle*,
    vtkCellArray*,
    vtkPoints*,
    vtkUnsignedCharArray*,
    vtkCellData*,
    QSet<const float*>*,
    QHash<vtkIdType, vtkIdType>* newIds = nullptr
  ) const;

  FiberIndexes m_Indices;
  unsigned int m_NbPoints;
  bool m_Enabled;
};

struct ImekaFiber_EXPORT FiberMapperData
{
  typedef std::map<const mitk::DataNode*, PartMapperData> TGMapperData;

  PartMapperData wholeMapperData;
  TGMapperData partsMapperData;
  bool useWhole;

  mitk::FilteredFiberBundle::Pointer fiberBundle;

  unsigned int TotalNbPoints() const;
  // The streamline groups of the new fiber bundle are the visible tract
  // groups (see below)
  mitk::FilteredFiberBundle::Pointer FiberBundleFromData(
    const bool, const bool) const;
  // `groups`, if given, receives the streamline groups of the written
  // streamlines, with indices in `newLines`:
  // - when only the whole dataset is displayed (no active tract group), the
  //   groups stored in the fiber bundle (e.g. loaded from a TRX file);
  // - otherwise, one group per enabled and visible tract group, named after
  //   its node.
  // Empty groups are not added. Names already in `groups` are made unique.
  void FiberBundleFromData(
    vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*, vtkCellData*,
    mitk::FilteredFiberBundle::StreamlineGroups* groups = nullptr) const;

  void SetIndices(const mitk::DataNode*, const FiberIndexes& indices);
  const std::vector<unsigned int>& GetIBO() const { return m_IBO; }
  void ClearIndices() { m_IBO.clear(); }

private:
  std::vector<unsigned int> m_IBO;
};

// Adds a group to `groups`, renaming it "name (2)", "name (3)", ... if the
// name is already used. Does nothing if `indices` is empty.
void ImekaFiber_EXPORT AddStreamlineGroup(
  mitk::FilteredFiberBundle::StreamlineGroups& groups,
  const std::string& name,
  std::vector<vtkIdType> indices);

mitk::FilteredFiberBundle::Pointer ImekaFiber_EXPORT NewFiberBundle(
  vtkSmartPointer<vtkPolyData>, vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED
