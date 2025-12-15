#ifndef IMEKA_FILTERED_FIBER_BUNDLE_HPP_INCLUDED
#define IMEKA_FILTERED_FIBER_BUNDLE_HPP_INCLUDED

#include <QSet>
#include <QStringList>

#include "mitkFiberBundle.h"

#include "FiberBundleExports.h"

class vtkPolyData;

namespace mitk
{

class LevelWindow;
class LookupTable;

class FiberBundle_EXPORT FilteredFiberBundle : public FiberBundle
{
public:
  enum Coloring
  {
    Orientation, EndPoint, Curvature, FromAnat, Uniform, FromFile
  };
  Coloring GetCurrentColoring() const  { return m_CurrentColoring; }
  bool IsInOrientationColoring() const { return m_CurrentColoring == Coloring::Orientation; }
  bool IsInEndPointColoring() const { return m_CurrentColoring == Coloring::EndPoint; }
  bool IsInCurvatureColoring() const { return m_CurrentColoring == Coloring::Curvature; }
  bool IsInFromAnatColoring() const { return m_CurrentColoring == Coloring::FromAnat; }
  bool IsInUniformColoring() const { return m_CurrentColoring == Coloring::Uniform; }
  bool IsInFromFileColoring() const { return m_CurrentColoring == Coloring::FromFile; }

  QStringList Filters() const;
  QString Filter() const;

  mitkClassMacro(FilteredFiberBundle, FiberBundle)
  itkFactorylessNewMacro(Self)
  itkCloneMacro(Self)
  mitkNewMacro1Param(Self, vtkPolyData*)
  mitkNewMacro3Param(Self, vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*)
  mitkNewMacro1Param(Self, const bool)

  void ExportDataTo(
    vtkCellArray*, vtkPoints*, vtkUnsignedCharArray* = nullptr) const;
  void ExportDataTo(
    vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*,
    const QSet<vtkIdType>&) const;
  void CalculateStatsUsingVisibility();
  float GetRealMaxFiberLength() const { return m_RealMaxFiberLength; }
  void MirrorFibers(unsigned int axis) {FiberBundle::MirrorFibers(axis);};
  void MirrorFibers(const BaseGeometry* anatGeo, const unsigned int axis);

  void ApplyColorCoding(
    mitk::DataNode*,
    const mitk::DataNode*,
    const FilteredFiberBundle::Coloring);

  void ColorCodingByEndPoints();
  virtual void ColorFibersByCurvature(const bool minMaxNorm = true);
  void ColorFibersByOrientation();
  template <typename TPixel>
  void ColorFibersByLUT(
    const mitk::PixelType pixelType,
    const mitk::Image*,
    vtkLookupTable*,
    const mitk::LevelWindow&);
  void ColorFibersByRGBAnat(const mitk::Image*, const mitk::LevelWindow&);
  void ColorFromAnat(const mitk::DataNode*);
  void ColorFibersByArray(vtkFloatArray*);
  void SetFiberColors(float r, float g, float b, float alpha = 255) override;
  void SetFiberColors(
    vtkSmartPointer<vtkUnsignedCharArray> fiberColors) override;
  void UpdateColorCoding(const mitk::DataNode*);
  // Delete all color information and set coloring to Orientation
  void DeleteColoring();

  void Shuffle();

  bool IsRTT() const { return m_RTT; }

  void SetFibersVisibility(const QSet<vtkIdType>& v)  { m_Visibility = v; }
  QSet<vtkIdType>& GetFibersVisibility() { return m_Visibility; }

protected:
  FilteredFiberBundle(vtkPolyData* fiberPolyData = nullptr);
  FilteredFiberBundle(vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*);
  FilteredFiberBundle(const bool RTT);
  virtual ~FilteredFiberBundle();

private:
  void ExportDataRTT(
    vtkCellArray*, vtkPoints*, vtkUnsignedCharArray*) const;
  void SetCurrentColorCoding(const Coloring);

  bool m_RTT;
  Coloring m_CurrentColoring;
  QSet<vtkIdType> m_Visibility;
  float m_RealMaxFiberLength;
};

} // namespace mitk

#endif // IMEKA_FILTERED_FIBER_BUNDLE_HPP_INCLUDED
