
#ifndef IMEKA_MAPPER_RGB_MAPPER_HPP_INCLUDED
#define IMEKA_MAPPER_RGB_MAPPER_HPP_INCLUDED

#include <mitkImageCast.h>
#include <mitkImageVtkMapper2D.h>

#include <itkImageLinearConstIteratorWithIndex.h>

#include "ImekaCommonExports.h"

namespace Imeka
{

namespace Mapper
{

// Swaps the standard image for an RGB version just for display.
class ImekaCommon_EXPORT ImekaRGBMapper : public mitk::ImageVtkMapper2D
{
public:
  mitkClassMacro(ImekaRGBMapper, ImageVtkMapper2D)
  itkNewMacro(Self);

  void SetDataNode(mitk::DataNode*) override;
  virtual const mitk::Image* GetInput();

protected:
  ImekaRGBMapper();
  ~ImekaRGBMapper() {}

private:
  mitk::Image::Pointer m_RGBImage;
};

template<typename TPixel, unsigned int VImageDimension>
void ToRGBImage(const itk::Image<TPixel, VImageDimension>* image,
  mitk::Image::Pointer& result)
{
  typedef itk::Image<TPixel, VImageDimension> ImageType;
  typename ImageType::RegionType inputImageRegion =
    image->GetLargestPossibleRegion();

  typedef itk::RGBPixel<unsigned char> RGB;
  typedef itk::Image<RGB, 3> RGBImageType;
  typename RGBImageType::RegionType rgbRegion;
  rgbRegion.SetSize(0, inputImageRegion.GetSize(0));
  rgbRegion.SetSize(1, inputImageRegion.GetSize(1));
  rgbRegion.SetSize(2, inputImageRegion.GetSize(2));
  RGBImageType::Pointer rgbImage = RGBImageType::New();
  rgbImage->SetRegions(rgbRegion);
  rgbImage->Allocate();

  typedef itk::ImageLinearConstIteratorWithIndex<ImageType> IteratorType;
  typename ImageType::IndexType index4D;
  typename RGBImageType::IndexType indexRGB;

  typename ImageType::RegionType region = image->GetBufferedRegion();
  IteratorType it(image, region);
  it.SetDirection(3);
  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    it.GoToBeginOfLine();
    RGB color;
    index4D = it.GetIndex();
    color.SetRed(it.Get());
    ++it;
    color.SetGreen(it.Get());
    ++it;
    color.SetBlue(it.Get());
    indexRGB[0] = index4D[0];
    indexRGB[1] = index4D[1];
    indexRGB[2] = index4D[2];
    rgbImage->SetPixel(indexRGB, color);
    it.NextLine();
  }

  mitk::CastToMitkImage(rgbImage, result);
}

} // namespace Mapper

} // namespace Imeka

#endif // IMEKA_MAPPER_RGB_MAPPER_HPP_INCLUDED
