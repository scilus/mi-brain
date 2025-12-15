#include "Surfaces.hpp"

#include <mitkManualSegmentationToSurfaceFilter.h>
#include <mitkSurfaceToImageFilter.h>

namespace Imeka
{

namespace Surface
{

mitk::Surface::Pointer ImageToSurface(
  const mitk::Image* anat,
  const mitk::Image* image)
{
  auto surfaceFilter = mitk::ManualSegmentationToSurfaceFilter::New();
  surfaceFilter->SetInput(image);
  surfaceFilter->SetThreshold(0.1);
  surfaceFilter->SetUseGaussianImageSmooth(false);
  surfaceFilter->SetSmooth(false);
  surfaceFilter->SetMedianFilter3D(false);
  surfaceFilter->SetDecimate(mitk::ImageToSurfaceFilter::NoDecimation);
  const auto spacing = anat->GetGeometry()->GetSpacing();
  surfaceFilter->SetInterpolation(spacing[0], spacing[1], spacing[2]);
  surfaceFilter->UpdateLargestPossibleRegion();
  mitk::Surface::Pointer surface = surfaceFilter->GetOutput();

  auto polyData = surface->GetVtkPolyData();
  polyData->SetVerts(nullptr);
  polyData->SetLines(nullptr);

  return surface;
}

mitk::Image::Pointer SurfaceToImage(
  const mitk::Image* anat,
  const mitk::Surface* surface)
{
  auto surfaceToImage = mitk::SurfaceToImageFilter::New();
  surfaceToImage->SetBackgroundValue(0.0);
  surfaceToImage->SetMakeOutputBinary(true);
  surfaceToImage->SetImage(anat);
  surfaceToImage->SetInput(surface);
  surfaceToImage->SetTolerance(0.1);
  surfaceToImage->GenerateData();
  surfaceToImage->Update();
  return surfaceToImage->GetOutput();
}

} // namespace Surface

} // namespace Imeka
