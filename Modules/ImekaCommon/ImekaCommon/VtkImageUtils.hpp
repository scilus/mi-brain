
#ifndef IMEKA_COMMON_VTK_IMAGE_UTILS_HPP_INCLUDED
#define IMEKA_COMMON_VTK_IMAGE_UTILS_HPP_INCLUDED

#include <itkRescaleIntensityImageFilter.h>

namespace Imeka
{

namespace Common
{

template<typename Pixel, unsigned int Dim>
void RescaleToFloat(
  itk::Image<Pixel, Dim>* image,
  typename itk::Image<float, Dim>::Pointer& outputImage)
{
  typedef itk::RescaleIntensityImageFilter<
    itk::Image<Pixel, Dim>,
    itk::Image<float, Dim>
  > ImageRescaler;
  typename ImageRescaler::Pointer rescaler = ImageRescaler::New();
  rescaler->SetInput(image);
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(1);
  rescaler->Update();

  outputImage = rescaler->GetOutput();
}

} // namespace Common

} // namespace Imeka

#endif // IMEKA_COMMON_VTK_IMAGE_UTILS_HPP_INCLUDED
