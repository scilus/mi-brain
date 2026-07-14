#include "RGBNiftiFileReader.hpp"

#include <itkImageFileReader.h>
#include <itkVectorImage.h>
#include <itkImageRegionIterator.h>
#include <itkVariableLengthVector.h>

#include <mitkFileReaderRegistry.h>
#include <mitkImage.h>
#include <mitkIOConstants.h>

#include <mitkImageCast.h>
#include <mitkImage.h>
#include <mitkLogMacros.h>
#include <mitkProperties.h>

#include "../MimeType.hpp"

namespace Imeka
{

namespace IO
{

RGBNiftiFileReader::RGBNiftiFileReader()
  : mitk::AbstractFileReader(
      GetRGBNiftiImageMimeType(),
      GetRGBNiftiImageDescription())
{
  this->SetDescription("4D NIfTI as RGB image");
  this->SetRanking(1000);
  // this->SetDefault(true);
  m_ServiceReg = this->RegisterService();
}

RGBNiftiFileReader::RGBNiftiFileReader(const RGBNiftiFileReader &other)
  : mitk::AbstractFileReader(other)
{}

// Verifies that the file is a 4D NIfTI, based on the verification that was done with GetRGBPredicate() for the old mapper.
// This makes it so that the reader does not support regular NIfTI files.
mitk::IFileIO::ConfidenceLevel
RGBNiftiFileReader::GetConfidenceLevel() const{
  const auto filename = this->GetInputLocation();

  if(filename.empty())
    return Unsupported;

  try {
    auto io = itk::ImageIOFactory::CreateImageIO(
      filename.c_str(),
      itk::IOFileModeEnum::ReadMode);

    if(!io) return Unsupported;

    io->SetFileName(filename);
    io->ReadImageInformation();

    if(io->GetNumberOfDimensions() != 4) return Unsupported;
    if(io->GetDimensions(3) != 3) return Unsupported;
    if(io->GetComponentType() != itk::IOComponentEnum::UCHAR) return Unsupported;

    return Supported;
  }
  catch (mitk::Exception) {
    return Unsupported;
  }
}

RGBNiftiFileReader* RGBNiftiFileReader::Clone() const
{
  return new RGBNiftiFileReader(*this);
}

std::vector<mitk::BaseData::Pointer> RGBNiftiFileReader::DoRead()
{
  std::vector<mitk::BaseData::Pointer> result;

  auto filename = this->GetInputLocation();
  if (filename.empty())
  {
    MITK_ERROR << "RGBNiftiFileReader: empty input filename";
    return result;
  }

  using Scalar4D = itk::Image<unsigned char, 4>;
  using Reader = itk::ImageFileReader<Scalar4D>;

  Reader::Pointer reader = Reader::New();
  reader->SetFileName(filename);

  try
  {
    reader->Update();
  }
  catch (const itk::ExceptionObject &e)
  {
    MITK_ERROR << "Failed to read file: " << e;
    return result;
  }

  Scalar4D::Pointer img4d = reader->GetOutput();
  Scalar4D::SizeType size4 = img4d->GetLargestPossibleRegion().GetSize();

  const auto x = static_cast<unsigned int>(size4[0]);
  const auto y = static_cast<unsigned int>(size4[1]);
  const auto z = static_cast<unsigned int>(size4[2]);
  const auto t = static_cast<unsigned int>(size4[3]);

  if (t < 3)
  {
    // just a precaution, this should not happen because the reader should only be called if the file is a 4D NIfTI with 3 channels in the last dimension.
    MITK_INFO << "t smaller than 3.\n";
    mitkThrow() << "Not an RGB NIfTI";
  }

  using RGBPixelType = itk::RGBPixel<unsigned char>;
  using RGBImageType = itk::Image<RGBPixelType, 3>;
  RGBImageType::Pointer rgb = RGBImageType::New();

  itk::Size<3> size3;
  size3[0] = x;
  size3[1] = y;
  size3[2] = z;

  itk::ImageRegion<3> region3d;
  region3d.SetSize(size3);

  rgb->SetRegions(region3d);
  rgb->Allocate();

  itk::ImageRegionIterator<RGBImageType> outIt(rgb, region3d);

  for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
  {
    itk::Index<3> idx3 = outIt.GetIndex();
    itk::Index<4> idx4;
    idx4[0] = idx3[0];
    idx4[1] = idx3[1];
    idx4[2] = idx3[2];

    RGBPixelType pixel;

    idx4[3] = 0;
    pixel.SetRed(img4d->GetPixel(idx4));
    idx4[3] = 1;
    pixel.SetGreen(img4d->GetPixel(idx4));
    idx4[3] = 2;
    pixel.SetBlue(img4d->GetPixel(idx4));

    outIt.Set(pixel);
  }

  mitk::Image::Pointer mitkImage;
  mitk::CastToMitkImage(rgb, mitkImage);

  auto originalMitkImage = mitk::Image::New();
  mitk::CastToMitkImage(img4d, originalMitkImage);

  // Copy the geometry from the original image to the new RGB image, so that it is displayed in the correct position and orientation.
  if(originalMitkImage->GetGeometry()){
    mitkImage->SetGeometry(
      originalMitkImage->GetGeometry()->Clone());
  }

  // Copy the time geometry from the original image to the new RGB image, so that it is displayed in the time frames.
  // Commented out because it seems to cause issues with the display of the RGB image in some cases. The RGB image is not a time series, so it should not have a time geometry.
  // if(originalMitkImage->GetTimeGeometry()){
  //   mitkImage->SetTimeGeometry(
  //     originalMitkImage->GetTimeGeometry()->Clone());
  // }

  mitkImage->SetProperty("binary", mitk::BoolProperty::New(false));
  result.push_back(mitkImage.GetPointer());
  return result;
}

} // namespace IO

} // namespace Imeka
