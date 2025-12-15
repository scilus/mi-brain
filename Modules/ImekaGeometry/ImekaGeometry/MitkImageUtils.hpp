
#ifndef IMEKA_GEOMETRY_MITK_IMAGE_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_MITK_IMAGE_UTILS_HPP_INCLUDED

#include <mitkImage.h>
#include <mitkImageCast.h>
#include <mitkITKImageImport.h>

#include "ImekaGeometryExports.h"

namespace Imeka
{

namespace Geometry
{

template <class ImageType>
mitk::Image::Pointer MitkImageFromItkImage(
  const ImageType& itkImage,
  const mitk::Vector3D& spacing,
  const mitk::Point3D& origin,
  const bool copyData)
{
  mitk::Image::Pointer im = nullptr;
  if (copyData)
  {
    im = mitk::Image::New();
    mitk::CastToMitkImage(itkImage, im);
  }
  else
  {
    im = mitk::ImportItkImage(itkImage);
  }

  im->GetGeometry()->SetSpacing(spacing);
  im->GetGeometry()->SetOrigin(origin);

  return im;
}

mitk::Image::Pointer ImekaGeometry_EXPORT GetImageForSelectedTime(
  const mitk::Image* image, const unsigned int t);

} // namespace Geometry

} // namespace Imeka

#endif // IMEKA_GEOMETRY_MITK_IMAGE_UTILS_HPP_INCLUDED
