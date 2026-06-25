
#include "MimeType.hpp"

#include <mitkIOMimeTypes.h>

namespace Imeka
{

namespace IO
{

std::string ImekaIO_EXPORT GetBoundingObjectName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".imeka.boundingobject";
}

std::string ImekaIO_EXPORT GetBoundingObjectDescription()
{
  return "Bounding Objects";
}

mitk::CustomMimeType ImekaIO_EXPORT GetBoundingObjectMimeType()
{
  mitk::CustomMimeType mimeType(GetBoundingObjectName());
  mimeType.AddExtension("bdo");
  mimeType.SetCategory("Surfaces");
  mimeType.SetComment("Imeka Bounding Object");
  return mimeType;
}

QString ImekaIO_EXPORT GetBoundingObjectFilter()
{
  const auto mime = GetBoundingObjectMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

std::string ImekaIO_EXPORT GetDataNodeGroupName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".imeka.datanodegroup";
}

std::string ImekaIO_EXPORT GetDataNodeGroupDescription()
{
  return "Datanode Groups";
}

mitk::CustomMimeType ImekaIO_EXPORT GetDataNodeGroupMimeType()
{
  mitk::CustomMimeType mimeType(GetDataNodeGroupName());
  mimeType.AddExtension("dng");
  mimeType.SetCategory(mitk::IOMimeTypes::CATEGORY_SURFACES());
  mimeType.SetComment("Imeka Data Node Group");
  return mimeType;
}

QString ImekaIO_EXPORT GetDataNodeGroupFilter()
{
  const auto mime = GetDataNodeGroupMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

std::string ImekaIO_EXPORT GetDicomGroupName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".image.dicom";
}

std::string ImekaIO_EXPORT GetDicomDescription()
{
  return "Dicom Images";
}

mitk::CustomMimeType ImekaIO_EXPORT GetDicomMimeType()
{
  mitk::CustomMimeType mimeType(GetDicomGroupName());
  mimeType.AddExtension("dcm");
  mimeType.SetCategory(mitk::IOMimeTypes::CATEGORY_IMAGES());
  mimeType.SetComment("Dicom Image");
  return mimeType;
}

QString ImekaIO_EXPORT GetDicomFilter()
{
  const auto mime = GetDicomMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

std::string ImekaIO_EXPORT GetFDFImageName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".image.fdf";
}

std::string ImekaIO_EXPORT GetFDFImageDescription()
{
  return "FDF Images";
}

mitk::CustomMimeType ImekaIO_EXPORT GetFDFImageMimeType()
{
  mitk::CustomMimeType mimeType(GetFDFImageName());
  mimeType.AddExtension("fdf");
  mimeType.SetCategory(mitk::IOMimeTypes::CATEGORY_IMAGES());
  mimeType.SetComment("FDF Image");
  return mimeType;
}

QString ImekaIO_EXPORT GetFDFImageFilter()
{
  const auto mime = GetFDFImageMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

std::string ImekaIO_EXPORT GetVFFImageName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".image.vff";
}

std::string ImekaIO_EXPORT GetVFFImageDescription()
{
  return "VFF Images";
}

mitk::CustomMimeType ImekaIO_EXPORT GetVFFImageMimeType()
{
  mitk::CustomMimeType mimeType(GetVFFImageName());
  mimeType.AddExtension("vff");
  mimeType.SetCategory(mitk::IOMimeTypes::CATEGORY_IMAGES());
  mimeType.SetComment("VFF Image");
  return mimeType;
}

QString ImekaIO_EXPORT GetVFFImageFilter()
{
  const auto mime = GetVFFImageMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

std::string ImekaIO_EXPORT GetRGBNiftiImageName()
{
  return mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".image.nifti.rgb";
}

std::string ImekaIO_EXPORT GetRGBNiftiImageDescription()
{
  return "RGB NIfTI Reader";
}

mitk::CustomMimeType ImekaIO_EXPORT GetRGBNiftiImageMimeType()
{
  mitk::CustomMimeType mimeType(GetRGBNiftiImageName());
  mimeType.AddExtension("nii");
  mimeType.AddExtension("nii.gz");
  mimeType.SetCategory(mitk::IOMimeTypes::CATEGORY_IMAGES());
  mimeType.SetComment("RGB NIfTI Image");
  return mimeType;
}

QString ImekaIO_EXPORT GetRGBNiftiImageFilter()
{
  const auto mime = GetRGBNiftiImageMimeType();
  return QString::fromStdString(
    mime.GetComment() + " (*." + mime.GetExtensions()[0] + ")"
  );
}

} // namespace IO

} // namespace Imeka
