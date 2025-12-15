
#ifndef IMEKA_FIBER_GLYPH_HPP_INCLUDED
#define IMEKA_FIBER_GLYPH_HPP_INCLUDED

#include <functional>
#include <limits>
#include <vector>

#include <mitkImage.h>
#include <mitkImageAccessByItk.h>
#include <mitkImageReadAccessor.h>
#include <mitkImageToItk.h>

#include "ImekaCommon/VtkImageUtils.hpp"
#include "ImekaGeometry/MitkImageUtils.hpp"
#include "../types.hpp"

namespace Imeka
{

namespace Fiber
{

const float NO_VALUE = std::numeric_limits<float>::infinity();

template <
  class PixelType,
  unsigned int Dimension,
  class Loader,
  class GlyphType = PixelType>
class Glyph
{
private:
  typedef itk::Image<PixelType, Dimension> ItkImageType;

public:
  Glyph()
    : m_Image(nullptr)
    , m_Step(0)
    , m_ImageX(0), m_ImageY(0), m_ImageZ(0)
  {}

  virtual ~Glyph() {}

  void SetImage(const mitk::Image* im)
  {
    if (im)
    {
      if (Dimension == 3 && im->GetDimension() > 3)
      {
        m_Image = Imeka::Geometry::GetImageForSelectedTime(im, 0);
      }
      else
      {
        m_Image = im;
      }

      m_Step = (im->GetDimension() < 4) ? 1 : im->GetDimension(3) / 3;
      m_ImageX = im->GetDimension(0);
      m_ImageY = im->GetDimension(1);
      m_ImageZ = im->GetDimension(2);

      Read();
    }
  }

  void Reload()
  {
    if (m_Image)
    {
      Read();
    }
  }

  unsigned long GetOffset(const mitk::Point3D& p) const
  {
    itk::Index<Dimension> idx;
    #pragma omp critical
    {
      m_Image->GetGeometry()->WorldToIndex(p, idx);
    }

    if (idx[0] >= 0 && idx[1] >= 0 && idx[2] >= 0
      && idx[0] < m_ImageX && idx[1] < m_ImageY && idx[2] < m_ImageZ)
    {
      return m_Step * (
        idx[2] * m_ImageX * m_ImageY + idx[1] * m_ImageX + idx[0]
      );
    }

    return std::numeric_limits<unsigned long>::max();
  }

  GlyphType Get(const unsigned long& offset) const
  {
    return m_Glyph[offset];
  }

protected:
  virtual void Read()
  {
    typename ItkImageType::Pointer itkImage = nullptr;
    if (m_Image->GetPixelType().GetComponentTypeAsString() != "float")
    {
      auto nonConstImage = const_cast<mitk::Image*>(m_Image.GetPointer());
      AccessFixedDimensionByItk_n(
        nonConstImage, Imeka::Common::RescaleToFloat, Dimension, (itkImage));
    }
    else
    {
      typename mitk::ImageToItk<ItkImageType>::Pointer imageToItk =
        mitk::ImageToItk<ItkImageType>::New();
      imageToItk->SetInput(m_Image);
      imageToItk->Update();
      itkImage = imageToItk->GetOutput();
    }

    const unsigned int nbElements = m_Step * m_ImageX * m_ImageY * m_ImageZ;
    m_Glyph = m_Loader.Load(itkImage, nbElements);
  }

  Loader m_Loader;
  mitk::Image::ConstPointer m_Image;

  unsigned int m_Step, m_ImageX, m_ImageY, m_ImageZ;

  std::vector<GlyphType> m_Glyph;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_GLYPH_HPP_INCLUDED
