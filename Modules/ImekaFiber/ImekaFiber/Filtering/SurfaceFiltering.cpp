
#include "SurfaceFiltering.hpp"

#include <itkImage.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryDilateImageFilter.h>

#include <mitkImageCast.h>
#include <mitkITKImageImport.h>

#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkOctreePointLocator.h>

#include "ImekaGeometry/VtkImageUtils.hpp"
#include "ImekaFiber/Filtering/common.hpp"
#include "ImekaFiber/Surfaces.hpp"

namespace Imeka
{

namespace Fiber
{

SurfaceFiltering::SurfaceFiltering()
  : m_Anat(nullptr)
{}

void SurfaceFiltering::SetAnat(const mitk::Image* anat)
{
  m_Anat = anat;
}

void SurfaceFiltering::Update(
  const mitk::FiberBundle* fiber,
  const DatasetFilteringData& datasetData,
  const mitk::BaseData* ROI,
  Result& results)
{
  mitk::Image::ConstPointer image = dynamic_cast<const mitk::Image*>(ROI);
  mitk::Surface::ConstPointer surface =
    dynamic_cast<const mitk::Surface*>(ROI);
  const bool surfaceOrMask =
    (image != nullptr)
    ^ // Either image OR surface
    (surface && !dynamic_cast<const SelectionObject*>(ROI));
  if (!surfaceOrMask) { return; }

  if (image.IsNotNull())
  {
    surface = Imeka::Surface::ImageToSurface(m_Anat, image);
  }
  else
  {
    image = Imeka::Surface::SurfaceToImage(m_Anat, surface);
  }

  // Geometry of the real refenrece anatomy OR of the current ROI if it's a
  // binary mask.
  auto anatGeo = image->GetGeometry();

  const unsigned int* imageDim = image->GetDimensions();
  m_ImageSize.SetElement(0, imageDim[0]);
  m_ImageSize.SetElement(1, imageDim[1]);
  m_ImageSize.SetElement(2, imageDim[2]);

  QSet<unsigned int> voxelsOriginal, voxelsDilate;
  const bool maskIsFilled = PrepareSurface(
    datasetData, image, voxelsOriginal, voxelsDilate);

  auto bounds = surface->GetGeometry()->GetBounds();
  double extendedBounds[] = {
    bounds[0] - datasetData.additionalBoundingMargin,
    bounds[1] + datasetData.additionalBoundingMargin,
    bounds[2] - datasetData.additionalBoundingMargin,
    bounds[3] + datasetData.additionalBoundingMargin,
    bounds[4] - datasetData.additionalBoundingMargin,
    bounds[5] + datasetData.additionalBoundingMargin };

  if (maskIsFilled) {
    std::cout << "Mask almost fills the anatomy. You should reverse your mask.\n\n";
    const auto p1 = anatGeo->GetCornerPoint(0);
    const auto p2 = anatGeo->GetCornerPoint(7);
    bounds[0] = extendedBounds[0] = p1[0];
    bounds[1] = extendedBounds[1] = p2[0];
    bounds[2] = extendedBounds[2] = p1[1];
    bounds[3] = extendedBounds[3] = p2[1];
    bounds[4] = extendedBounds[4] = p1[2];
    bounds[5] = extendedBounds[5] = p2[2];
  }

  auto octree = datasetData.octreeFuture.result();

  auto pointsIDs = vtkSmartPointer<vtkIdTypeArray>::New();
  octree->FindPointsInArea(bounds.Begin(), pointsIDs);

  auto extendedPointsIDs = vtkSmartPointer<vtkIdTypeArray>::New();
  octree->FindPointsInArea(extendedBounds, extendedPointsIDs);

  vtkPolyData* fiberPolyData = fiber->GetFiberPolyData();
  fiberPolyData->BuildLinks();

  auto presentFibers = FibersWithPointsInSurface(
    fiberPolyData, pointsIDs, voxelsOriginal, anatGeo);
  DetectIntersectionsWithSurface(
    fiberPolyData, presentFibers, extendedPointsIDs, surface, voxelsDilate,
    anatGeo);

  results = Result(presentFibers);
}

bool SurfaceFiltering::PrepareSurface(
  const DatasetFilteringData& datasetData,
  const mitk::Image* image,
  QSet<unsigned int>& voxelsOriginal,
  QSet<unsigned int>& voxelsDilate) const
{
  typedef itk::Image<float, 3> ImageBinaryType;
  auto itkImage = ImageBinaryType::New();
  CastToItkImage(image, itkImage);

  // Heuristic to check the 8 corners to know if the mask "fills" the edges.
  // The mask is probably not filled for real. As explained in #1412, we simply
  // want to avoid the case where the mask "starts" at the edge, otherwise the
  // the surface produced from the mask will be reversed.
  typedef typename ImageBinaryType::IndexValueType IndexValueType;
  const IndexValueType width = m_ImageSize[0] - 1;
  const IndexValueType height = m_ImageSize[1] - 1;
  const IndexValueType depth = m_ImageSize[2] - 1;
  const auto isFilled =
    itkImage->GetPixel({{ 0, 0, 0 }}) &&
    itkImage->GetPixel({{ 0, 0, depth }}) &&
    itkImage->GetPixel({{ 0, height, 0 }}) &&
    itkImage->GetPixel({{ 0, height, depth }}) &&
    itkImage->GetPixel({{ width, 0, 0 }}) &&
    itkImage->GetPixel({{ width, 0, depth }}) &&
    itkImage->GetPixel({{ width, height, 0 }}) &&
    itkImage->GetPixel({{ width, height, depth }});

  unsigned int nbNonNullVoxels = 0;
  Imeka::Geometry::GetNbNonNullVoxels<float, 3>(
    itkImage, nbNonNullVoxels);
  voxelsOriginal.reserve(nbNonNullVoxels);
  Imeka::Geometry::GetNonNullIndexes<float, 3>(
    itkImage, voxelsOriginal);

  typedef itk::BinaryBallStructuringElement<
    ImageBinaryType::PixelType, 3> BallType;
  BallType binaryBall;
  binaryBall.SetRadius(static_cast<itk::SizeValueType>(
    datasetData.additionalBoundingMargin + 0.5));
  binaryBall.CreateStructuringElement();

  typedef itk::BinaryDilateImageFilter<
    ImageBinaryType, ImageBinaryType, BallType> DilationFilterType;
  auto dilationFilter = DilationFilterType::New();
  dilationFilter->SetInput(itkImage);
  dilationFilter->SetKernel(binaryBall);
  dilationFilter->SetDilateValue(1);
  dilationFilter->UpdateLargestPossibleRegion();
  ImageBinaryType* dilatedImage = dilationFilter->GetOutput();

  nbNonNullVoxels = 0;
  Imeka::Geometry::GetNbNonNullVoxels<float, 3>(
    dilatedImage, nbNonNullVoxels);
  voxelsDilate.reserve(nbNonNullVoxels);
  Imeka::Geometry::GetUniqueNonNullIndexes<float, 3>(
    dilatedImage, voxelsOriginal, voxelsDilate);

  return isFilled;
}

SelectedByROIFiberIndexes SurfaceFiltering::FibersWithPointsInSurface(
  vtkPolyData* fiberPolyData,
  vtkIdTypeArray* pointsIdsInBounds,
  const QSet<unsigned int>& voxelsOriginal,
  mitk::BaseGeometry* anatGeo)
{
  SelectedByROIFiberIndexes fiberIndexes;
  auto cellIds = vtkSmartPointer<vtkIdList>::New();
  const vtkIdType nbPoints = pointsIdsInBounds->GetNumberOfTuples();
  for (int i = 0; i < nbPoints; ++i)
  {
    const auto pointID = pointsIdsInBounds->GetValue(i);

    fiberPolyData->GetPointCells(pointID, cellIds);
    const auto currentFiberIndex = cellIds->GetId(0);

    mitk::Point3D p; itk::Index<3> idx;
    fiberPolyData->GetPoint(pointID, p.Begin());
    anatGeo->WorldToIndex(p, idx);

    if (voxelsOriginal.contains(
      Imeka::Geometry::ConvertIndexToInt(idx, m_ImageSize)))
    {
      fiberIndexes.anyPartThroughROI.insert(currentFiberIndex);

      // Warning (Pos) is on the streamline level, (Id) is on the fiber level
      fiberPolyData->GetCellPoints(currentFiberIndex, cellIds);
      const vtkIdType nbPointsOnStreamline = cellIds->GetNumberOfIds();
      const vtkIdType pointPosition = cellIds->IsId(pointID);

      // Is the first End selected?
      if (pointPosition == 0)
      {
        fiberIndexes.startInROI.insert(currentFiberIndex);
      }
      // Is the last End selected?
      if (pointPosition == nbPointsOnStreamline - 1)
      {
        fiberIndexes.endInROI.insert(currentFiberIndex);
      }
    }
  }
  return fiberIndexes;
}

void SurfaceFiltering::DetectIntersectionsWithSurface(
  vtkPolyData* fiberPolyData,
  SelectedByROIFiberIndexes& fiberIndexes,
  vtkIdTypeArray* pointsIdsInBounds,
  const mitk::Surface* surface,
  const QSet<unsigned int>& voxelsDilate,
  mitk::BaseGeometry* anatGeo)
{
  auto tree = vtkSmartPointer<vtkCellLocator>::New();
  tree->SetDataSet(surface->GetVtkPolyData());
  tree->BuildLocator();

  auto cellIds = vtkSmartPointer<vtkIdList>::New();
  auto genericCell = vtkSmartPointer<vtkGenericCell>::New();
  const vtkIdType nbPoints = pointsIdsInBounds->GetNumberOfTuples();
  for (int i = 0; i < nbPoints; ++i)
  {
    const auto pointID = pointsIdsInBounds->GetValue(i);
    fiberPolyData->GetPointCells(pointID, cellIds);
    const auto currentFiberIndex = cellIds->GetId(0);

    mitk::Point3D p; itk::Index<3> idx;
    fiberPolyData->GetPoint(pointID, p.Begin());
    anatGeo->WorldToIndex(p, idx);

    const bool isAlreadyIn =
      fiberIndexes.anyPartThroughROI.contains(currentFiberIndex);
    if (!isAlreadyIn && voxelsDilate.contains(
      Imeka::Geometry::ConvertIndexToInt(idx, m_ImageSize)))
    {
      // If one (or both) intersections tests succeeded,
      // add the current streamline
      const bool segmentsIntersectBox = IntersectionExists(
        currentFiberIndex, fiberPolyData, pointID, tree, genericCell);
      if (segmentsIntersectBox)
      {
        fiberIndexes.anyPartThroughROI.insert(currentFiberIndex);
      }
    }
  }
}

} // namespace Fiber

} // namespace Imeka
