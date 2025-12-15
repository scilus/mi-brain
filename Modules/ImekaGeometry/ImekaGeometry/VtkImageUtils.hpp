
#ifndef IMEKA_GEOMETRY_VTK_IMAGE_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_VTK_IMAGE_UTILS_HPP_INCLUDED

#include <numeric>

#include <QSet>

#include <itkImage.h>
#include <itkImageLinearConstIteratorWithIndex.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageSliceConstIteratorWithIndex.h>

#include <mitkBaseGeometry.h>
#include <mitkImage.h>

namespace Imeka
{

namespace Geometry
{

inline unsigned int ConvertIndexToInt(
  const itk::Index<3>& idx,
  const itk::Size<3>& size)
{
  return (size[0] * size[1]) * (idx[2]) + (size[1] * (idx[1])) + idx[0];
}

/* Heuristic to check if a float image is probably an integer image. It only
   reads the value of the middle axial slice, ensures that there's no float
   values then return all the labels if it was the case. */
template<typename Pixel>
QSet<short> IsActuallyIntegers(
  const itk::Image<Pixel, 3>* im)
{
  typedef itk::Image<Pixel, 3> ImageType;

  typename ImageType::RegionType sliceRegion =
    im->GetLargestPossibleRegion();
  sliceRegion.GetModifiableIndex()[2] = sliceRegion.GetSize()[2] / 2;
  sliceRegion.GetModifiableSize()[2] = 1;

  itk::ImageSliceConstIteratorWithIndex<ImageType> it(im, sliceRegion);
  it.SetFirstDirection(0);
  it.SetSecondDirection(1);

  QSet<short> labels;
  it.GoToBegin();
  while (!it.IsAtEndOfSlice())
  {
    while (!it.IsAtEndOfLine())
    {
      const Pixel value = it.Get();
      if (static_cast<Pixel>(static_cast<int>(value) != value))
      {
        return QSet<short>();
      }
      if (value != 0.0)
      {
        labels.insert(value);
      }
      ++it;
    }
    it.NextLine();
  }

  return labels;
}

template<typename Pixel, unsigned int Dimension>
void GetNbNonNullVoxels(
  const itk::Image<Pixel, Dimension>* im,
  unsigned int& nb)
{
  typedef itk::Image<Pixel, Dimension> ImageType;
  itk::ImageRegionConstIterator<ImageType> it(im, im->GetRequestedRegion());
  it.GoToBegin();
  for (; !it.IsAtEnd(); ++it)
  {
    if (it.Get() > 0)
    {
      ++nb;
    }
  }
}

template<typename Pixel, unsigned int Dimension>
void GetNonNullPoints(
  const itk::Image<Pixel, Dimension>* im,
  mitk::BaseGeometry* geo,
  std::vector<mitk::Point3D>& points)
{
  typedef itk::Image<Pixel, Dimension> ImageType;
  itk::ImageLinearConstIteratorWithIndex<ImageType> it(
    im, im->GetRequestedRegion());
  it.SetDirection(0);
  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    while (!it.IsAtEndOfLine())
    {
      if (it.Get() > 0)
      {
        mitk::Point3D p;
        geo->IndexToWorld(it.GetIndex(), p);
        points.push_back(p);
      }
      ++it;
    }
    it.NextLine();
  }
}

// Overload of the function used in the SurfaceFiltering function named
// PrepareSurface
template<typename Pixel, unsigned int Dimension>
void GetNonNullIndexes(
  const itk::Image<Pixel, Dimension>* im,
  QSet<unsigned int>& indexes)
{
  typedef itk::Image<Pixel, Dimension> ImageType;
  itk::ImageLinearConstIteratorWithIndex<ImageType> it(
    im, im->GetRequestedRegion());
  it.SetDirection(0);
  it.GoToBegin();
  const itk::Size<3> imageSize = im->GetLargestPossibleRegion().GetSize();
  while (!it.IsAtEnd())
  {
    while (!it.IsAtEndOfLine())
    {
      if (it.Get() > 0)
      {
        indexes.insert(ConvertIndexToInt(it.GetIndex(), imageSize));
      }
      ++it;
    }
    it.NextLine();
  }
}

// Only insert indexes into 'indexes_2' that are not in 'indexes_1'.
// Equivalent to adding all indexes and then doing a substraction :
// indexes_2.substract(indexes_1);
template<typename Pixel, unsigned int Dimension>
void GetUniqueNonNullIndexes(
  const itk::Image<Pixel, Dimension>* im,
  QSet<unsigned int>& indexes_1, QSet<unsigned int>& indexes_2)
{
  typedef itk::Image<Pixel, Dimension> ImageType;
  itk::ImageLinearConstIteratorWithIndex<ImageType> it(
    im, im->GetRequestedRegion());
  it.SetDirection(0);
  it.GoToBegin();
  const itk::Size<3> imageSize = im->GetLargestPossibleRegion().GetSize();
  while (!it.IsAtEnd())
  {
    while (!it.IsAtEndOfLine())
    {
      const unsigned int value = ConvertIndexToInt(it.GetIndex(), imageSize);
      if (it.Get() > 0 && !indexes_1.contains(value))
      {
        indexes_2.insert(value);
      }
      ++it;
    }
    it.NextLine();
  }
}

// Create a new itk::Image of the required type. The intensities are left as-is
// you need to call im->FillBuffer(value) if you want a blank image.
template <class ImageType>
const typename ImageType::Pointer
CreateItkImage(const typename ImageType::SizeType &size)
{
  typename ImageType::IndexType start;
  start.Fill(0);

  typename ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  typename ImageType::Pointer im = ImageType::New();
  im->SetRegions(region);
  im->Allocate();
  return im;
}

} // namespace Geometry

} // namespace Imeka

#endif // IMEKA_GEOMETRY_VTK_IMAGE_UTILS_HPP_INCLUDED
