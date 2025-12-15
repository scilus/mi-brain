#ifndef IMEKA_FIBER_STREAMLINES_CUTTER_HPP_INCLUDED
#define IMEKA_FIBER_STREAMLINES_CUTTER_HPP_INCLUDED

#include <vtkCellArray.h>
#include <vtkOBBTree.h>
#include <vtkPolyLine.h>

#include "types.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

// Streamlines going through the SO will be cut (line-based)
class ImekaFiber_EXPORT StreamlinesCutter
{
public:
  StreamlinesCutter(
    const mitk::FilteredFiberBundle*, const SelectionObject*, const bool);
  ~StreamlinesCutter() {}

  mitk::FilteredFiberBundle::Pointer GenerateCuttedStreamlines(
    const FiberIndexes&, const bool);
  bool DidManageColors() const { return m_ManageColors; };

private:
  enum LineState { NothingSpecial, Intersect, Traverse };

  unsigned char* GetStreamlineColors(const vtkIdType streamlineIdx) const;

  // Cut individual streamlines and add it to vtk data structure
  void GenerateCuttedStreamline(const vtkIdType);

  LineState GetLineState(
    const mitk::Point3D&, const mitk::Point3D&, const bool, const bool) const;
  vtkSmartPointer<vtkPoints> GetIntersectingPoints(
    const mitk::Point3D&, const mitk::Point3D&) const;

  vtkIdType DoIntersect(
    const mitk::Point3D&, const mitk::Point3D&,
    const bool, const bool, const bool);
  vtkIdType DoTraverse(
    const mitk::Point3D&, const mitk::Point3D&);

  // m_NewStreamline, AddPoint and NewLine probably shouldn't be class members
  // but they were practical to increase readability and to avoid passing
  // parameters or lambdas.
  void AddPoint(
    const mitk::Point3D& p0,
    const mitk::Point3D& p,
    const mitk::Point3D& p1)
  {
    // Called without a color so it's an intersecting point, so we must
    // calculate the color of `p`, which is between p0 and p1.
    unsigned char intersectingColor[] = {
      m_P0Color[0], m_P0Color[1], m_P0Color[2],
      m_P0Color[3] // Alpha is always the same
    };

    if (m_ManageColors)
    {
      // For x, y and z, if colors are different, then interpolate using the
      // ratio of p's position between p0 and p1.
      for (unsigned int i = 0; i < 3; ++i)
      {
        if (m_P0Color[i] != m_P1Color[i])
        {
          const float ratio = fabs((p[i] - p0[i]) / (p1[i] - p0[i]));
          intersectingColor[i] =
            m_P0Color[i] + (m_P1Color[i] - m_P0Color[i]) * ratio;
        }
      }
    }

    AddPoint(p, intersectingColor);
  }
  void AddPoint(const mitk::Point3D& p, const unsigned char* colors)
  {
    m_NewStreamline->GetPointIds()->InsertNextId(
      m_NewPoints->InsertNextPoint(p.Begin()));
    if (m_ManageColors)
    {
      m_NewColors->InsertNextTypedTuple(colors);
    }
  }
  void NewLine()
  {
    const vtkIdType nbPoints = m_NewStreamline->GetNumberOfPoints();
    if (nbPoints == 1)
    {
      m_NewStreamline = vtkSmartPointer<vtkPolyLine>::New();
    }
    else if (nbPoints > 1)
    {
      m_NewCells->InsertNextCell(m_NewStreamline);
      m_NewStreamline = vtkSmartPointer<vtkPolyLine>::New();
    }
  }

  // If cutting from the outside, any streamlines not going through the SO
  // need to be added to the cutted one.
  void AddUncuttedStreamlines(const FiberIndexes&);

  const mitk::FilteredFiberBundle* m_OriginalFibers;
  vtkPolyData* m_OriginalPolyData;

  /*
     Colors will be managed in all cases but we need to make a special effort
     only on Color::FromFile because there's no way to calculate it after the
     new fibers is created. In the FromFile case, we interpolate from the known
     colors (see AddPoint), in all other cases, we simply call the right
     coloring method after.
  */
  const bool m_ManageColors;
  const unsigned char* m_OriginalColors;
  const unsigned char* m_P0Color, *m_P1Color;

  const SelectionObject* m_SO;
  const bool m_DrawOutside, m_DrawInside;

  vtkSmartPointer<vtkOBBTree> m_OBBTree;
  vtkSmartPointer<vtkPoints> m_NewPoints;
  vtkSmartPointer<vtkCellArray> m_NewCells;
  vtkSmartPointer<vtkUnsignedCharArray> m_NewColors;
  std::vector<bool> m_IsInside;
  vtkSmartPointer<vtkPolyLine> m_NewStreamline;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_STREAMLINES_CUTTER_HPP_INCLUDED
