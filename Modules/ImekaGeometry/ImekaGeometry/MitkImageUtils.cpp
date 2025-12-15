#include "MitkImageUtils.hpp"

#include <mitkImageTimeSelector.h>

namespace Imeka
{

namespace Geometry
{

mitk::Image::Pointer GetImageForSelectedTime(
  const mitk::Image* image,
  const unsigned int t)
{
  auto timeSelector = mitk::ImageTimeSelector::New();
  timeSelector->SetInput(image);
  timeSelector->SetTimeNr(t);
  timeSelector->UpdateLargestPossibleRegion();
  return timeSelector->GetOutput();
}

} // namespace Geometry

} // namespace Imeka
