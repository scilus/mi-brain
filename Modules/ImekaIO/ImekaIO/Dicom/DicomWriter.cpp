
#include "DicomWriter.hpp"

#include <mitkImage.h>
#include <mitkImageReadAccessor.h>

#include <itkImageIOBase.h>
#include <itkImageIOFactory.h>
#include <itkMetaDataObject.h>

#include "../MimeType.hpp"

namespace Imeka
{

namespace IO
{

DicomWriter::DicomWriter()
  : mitk::AbstractFileWriter(
      mitk::Image::GetStaticNameOfClass(),
      GetDicomMimeType(),
      GetDicomDescription())
{
  RegisterService();
}

DicomWriter::DicomWriter(const DicomWriter& other)
  : mitk::AbstractFileWriter(other)
{}

DicomWriter* DicomWriter::Clone() const
{
  return new DicomWriter(*this);
}

void DicomWriter::Write()
{
  MITK_INFO << "Writing image: " << GetOutputLocation() << std::endl;

  auto image = dynamic_cast<const mitk::Image*>(GetInput());

  // Implementation of writer using itkImageIO directly. This skips the use
  // of templated itkImageFileWriter, which saves the multiplexing on MITK side.

  unsigned int dimension = image->GetDimension();
  unsigned int* dimensions = image->GetDimensions();
  mitk::PixelType pixelType = image->GetPixelType();
  mitk::Vector3D mitkSpacing = image->GetGeometry()->GetSpacing();
  mitk::Point3D mitkOrigin = image->GetGeometry()->GetOrigin();

  // Due to templating in itk, we are forced to save a 4D spacing and 4D Origin, though they are not supported in MITK
  itk::Vector<double, 4u> spacing4D;
  spacing4D[0] = mitkSpacing[0];
  spacing4D[1] = mitkSpacing[1];
  spacing4D[2] = mitkSpacing[2];
  spacing4D[3] = 1; // There is no support for a 4D spacing. However, we should have an valid value here

  itk::Vector<double, 4u> origin4D;
  origin4D[0] = mitkOrigin[0];
  origin4D[1] = mitkOrigin[1];
  origin4D[2] = mitkOrigin[2];
  origin4D[3] = 0; // There is no support for a 4D origin. However, we should have an valid value here

  itk::ImageIOBase::Pointer imageIO =
    itk::ImageIOFactory::CreateImageIO(GetOutputLocation().c_str(), itk::IOFileModeEnum::WriteMode);

  if(imageIO.IsNull())
  {
    MITK_ERROR << "Error: Could not create itkImageIO via factory for file " << GetOutputLocation();
  }

  // Set the necessary information for imageIO
  imageIO->SetNumberOfDimensions(dimension);
  imageIO->SetPixelType( pixelType.GetPixelType() );
  imageIO->SetComponentType(
    (static_cast<int>(pixelType.GetComponentType()) < mitk::PixelComponentUserType) ?
      pixelType.GetComponentType() :
      itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE);
  imageIO->SetNumberOfComponents( pixelType.GetNumberOfComponents() );

  itk::ImageIORegion ioRegion( dimension );

  for(unsigned int i=0; i<dimension; i++)
  {
    imageIO->SetDimensions(i,dimensions[i]);
    imageIO->SetSpacing(i,spacing4D[i]);
    imageIO->SetOrigin(i,origin4D[i]);

    mitk::Vector3D mitkDirection;
    vnl_vector<double> column(image->GetGeometry()->GetIndexToWorldTransform()->GetMatrix().GetVnlMatrix().get_column(i));
    mitkDirection.SetVnlVector(column);
    itk::Vector<double, 4u> direction4D;
    direction4D[0] = mitkDirection[0];
    direction4D[1] = mitkDirection[1];
    direction4D[2] = mitkDirection[2];

    // MITK only supports a 3x3 direction matrix. Due to templating in itk, however, we must
    // save a 4x4 matrix for 4D images. in this case, add an homogneous component to the matrix.
    if (i == 3)
      direction4D[3] = 1; // homogenous component
    else
      direction4D[3] = 0;

    vnl_vector< double > axisDirection(dimension);
    for(unsigned int j=0; j<dimension; j++)
    {
      axisDirection[j] = direction4D[j]/spacing4D[i];
    }
    imageIO->SetDirection( i, axisDirection );

    ioRegion.SetSize(i, image->GetLargestPossibleRegion().GetSize(i) );
    ioRegion.SetIndex(i, image->GetLargestPossibleRegion().GetIndex(i) );
  }

  // Send some header information to itkGDCMImageIO using the
  // MetaDataDictionnary from itk::Object
  itk::MetaDataDictionary& dict = imageIO->GetMetaDataDictionary();
  const std::string SOP = "1.2.840.10008.5.1.4.1.1.4";
  itk::EncapsulateMetaData(dict, "0008|0016", SOP);

  //use compression if available
  imageIO->UseCompressionOn();

  imageIO->SetIORegion(ioRegion);
  imageIO->SetFileName(GetOutputLocation());

  mitk::ImageReadAccessor imageAccess(image);
  imageIO->Write(imageAccess.GetData());
}

} // namespace IO

} // namespace Imeka
