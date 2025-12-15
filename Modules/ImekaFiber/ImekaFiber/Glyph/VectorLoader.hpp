
#ifndef IMEKA_FIBER_GLYPH_VECTOR_LOADER_HPP_INCLUDED
#define IMEKA_FIBER_GLYPH_VECTOR_LOADER_HPP_INCLUDED

#include <itkImageLinearConstIteratorWithIndex.h>

#include <vector>

#include "../types.hpp"

namespace Imeka
{

namespace Fiber
{

// If the image is in 4D, then iterate on it using the fourth dimension
// as the main direction. We do that because it's more cache friendly
// (we use all data from the 4th dimension together)
template <class T, unsigned int D>
class VectorLoader
{
  typedef itk::Image<T, D> ImageType;

public:
  VectorLoader()
    : m_AnatGeometry(nullptr)
  {}

  void SetAffineTransform(mitk::BaseGeometry* anatGeo)
  {
    m_AnatGeometry = anatGeo;
  }

  mitk::BaseGeometry* GetAffineTransform() const
  {
    return m_AnatGeometry;
  }

  std::vector<V> Load(
    typename ImageType::Pointer im,
    const unsigned int nbElements)
  {
    std::vector<V> glyph;
    glyph.reserve(nbElements);

    mitk::ScalarType biggestNorm = 0.0;
    itk::ImageLinearConstIteratorWithIndex<ImageType> it(
      im, im->GetLargestPossibleRegion());
    it.SetDirection(3);
    it.GoToBegin();
    while (!it.IsAtEnd())
    {
      while (!it.IsAtEndOfLine())
      {
        const T x = it.Get(); ++it;
        const T y = it.Get(); ++it;
        const T z = it.Get(); ++it;

        V v;
        if (x != x || std::abs(x) + std::abs(y) + std::abs(z) < 0.01) // isnan or empty
        {
          v.Fill(NO_VALUE);
        }
        else
        {
          mitk::FillVector3D(v, x, y, z);
          if (m_AnatGeometry)
          {
            m_AnatGeometry->IndexToWorld(v, v);
            biggestNorm = std::max(v.GetNorm(), biggestNorm);
          }
        }
        glyph.push_back(v);
      }
      it.NextLine();
    }

    // Divide all peaks by the biggest norm
    if (m_AnatGeometry)
    {
      for (auto& v : glyph)
      {
        if (v[0] != NO_VALUE)
        {
          v *= (v.GetNorm() / biggestNorm);
        }
      }
    }

    return glyph;
  }

private:
  mitk::BaseGeometry* m_AnatGeometry;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_GLYPH_VECTOR_LOADER_HPP_INCLUDED

