// Must be the first included file or it doesn't work
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "Maxima.hpp"

#include "ImekaGeometry/MathUtils.hpp"

namespace Imeka
{

namespace Fiber
{

void Maxima::SetImage(const mitk::Image* im)
{
  m_PeaksFibers = nullptr;

  if (im)
  {
    m_Loader.SetAffineTransform(im->GetGeometry());
  }
  Glyph::SetImage(im);
}

bool Maxima::ValidOffset(const unsigned long offset) const
{
  return offset < m_Glyph.size()
    && m_Glyph[offset][0] != NO_VALUE;
}

V Maxima::PickDirection(const unsigned long offset) const
{
  // TODO I used a maximum of 3 here but it's not right. The real maximum
  // should be 5. The if at the end of this function should also be corrected.
  mitk::ScalarType norms[3] = { 0 };
  mitk::ScalarType sum = 0.0f;
  for (unsigned int i = 0; i < 3; ++i)
  {
    const V& v = m_Glyph[offset + i];
    if (v[0] == NO_VALUE) { break; }

    const auto norm = v.GetNorm();
    norms[i] = norm;
    sum += norm;
  }

  // TODO flippedAxes
  const auto random = static_cast<mitk::ScalarType>(rand())
    / static_cast<mitk::ScalarType>(RAND_MAX);
  const auto weight = random * sum;
  if (weight < norms[0])
  {
    return m_Glyph[offset];
  }
  else if (weight < norms[0] + norms[1])
  {
    return m_Glyph[offset + 1];
  }

  return m_Glyph[offset + 2];
}

V Maxima::AdvecIntegrate(
  const V& direction,
  const unsigned long offset,
  const mitk::ScalarType fa,
  const double& puncture) const
{
  V out;
  float angleMin = 360.0;

  for (unsigned int i = 0; i < 3; ++i)
  {
    V v = m_Glyph[offset + i];
    if (v[0] == NO_VALUE) { break; }

    v.Normalize();
    if (direction * v < 0) { v *= -1.0; }
    const auto angle = 180.0 * std::acos(v * direction) / M_PI;
    if (angle < angleMin)
    {
      angleMin = angle;
      out = v;
    }
  }

  return Normalize(
    fa * out + (1.0 - fa) * ((1.0 - puncture) * direction + puncture * out));
}

mitk::FilteredFiberBundle::Pointer Maxima::ToFiberBundle()
{
  if (m_PeaksFibers) { return m_PeaksFibers; }

  auto lines = vtkSmartPointer<vtkCellArray>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();

  const auto spacing = m_Image->GetGeometry()->GetSpacing();
  const auto halfSpacing = spacing * 0.495;
  mitk::Point3D index, world;

  auto geo = m_Loader.GetAffineTransform();

  unsigned int idx = 0;
  for (unsigned int z = 0; z < m_ImageZ; ++z)
  {
    index[2] = z;
    for (unsigned int y = 0; y < m_ImageY; ++y)
    {
      index[1] = y;
      for (unsigned int x = 0; x < m_ImageX; ++x)
      {
        index[0] = x;
        geo->IndexToWorld(index, world);

        for (unsigned int i = 0; i < m_Step; ++i, ++idx)
        {
          auto v = m_Glyph[idx];
          if (v[0] == NO_VALUE)
          {
            idx += m_Step - i;
            break;
          }

          // Set its size to half a voxel so it doesn't "spread"
          v.Normalize();
          v[0] *= halfSpacing[0];
          v[1] *= halfSpacing[1];
          v[2] *= halfSpacing[2];

          lines->InsertNextCell(2);
          lines->InsertCellPoint(
            points->InsertNextPoint((world - v).Begin()));
          lines->InsertCellPoint(
            points->InsertNextPoint((world + v).Begin()));
        }
      }
    }
  }

  m_PeaksFibers = mitk::FilteredFiberBundle::New(lines, points, nullptr);
  return m_PeaksFibers;
}

} // namespace Fiber

} // namespace Imeka
