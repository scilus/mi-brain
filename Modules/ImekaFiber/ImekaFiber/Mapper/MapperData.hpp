#ifndef IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED
#define IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED

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
  void FiberBundleFromData(
    mitk::FilteredFiberBundle*,
    vtkCellArray*,
    vtkPoints*,
    vtkUnsignedCharArray*,
    vtkCellData*,
    QSet<const float*>*
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
  mitk::FilteredFiberBundle::Pointer FiberBundleFromData(
    const bool, const bool) const;
  void FiberBundleFromData(
    vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*, vtkCellData*) const;

  void SetIndices(const mitk::DataNode*, const FiberIndexes& indices);
  const std::vector<unsigned int>& GetIBO() const { return m_IBO; }
  void ClearIndices() { m_IBO.clear(); }

private:
  std::vector<unsigned int> m_IBO;
};

mitk::FilteredFiberBundle::Pointer ImekaFiber_EXPORT NewFiberBundle(
  vtkSmartPointer<vtkPolyData>, vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_MAPPER_DATA_HPP_INCLUDED
