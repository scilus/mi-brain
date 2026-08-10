
#ifndef IMEKA_IO_MIME_TYPE_HPP_INCLUDED
#define IMEKA_IO_MIME_TYPE_HPP_INCLUDED

#include <mitkCustomMimeType.h>

#include <QString>

#include "ImekaIOExports.h"

namespace Imeka
{

namespace IO
{

std::string ImekaIO_EXPORT GetBoundingObjectName();
std::string ImekaIO_EXPORT GetBoundingObjectDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetBoundingObjectMimeType();
QString ImekaIO_EXPORT GetBoundingObjectFilter();

std::string ImekaIO_EXPORT GetDataNodeGroupName();
std::string ImekaIO_EXPORT GetDataNodeGroupDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetDataNodeGroupMimeType();
QString ImekaIO_EXPORT GetDataNodeGroupFilter();

std::string ImekaIO_EXPORT GetDicomGroupName();
std::string ImekaIO_EXPORT GetDicomDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetDicomMimeType();
QString ImekaIO_EXPORT GetDicomFilter();

std::string ImekaIO_EXPORT GetFDFImageName();
std::string ImekaIO_EXPORT GetFDFImageDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetFDFImageMimeType();
QString ImekaIO_EXPORT GetFDFImageFilter();

std::string ImekaIO_EXPORT GetVFFImageName();
std::string ImekaIO_EXPORT GetVFFImageDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetVFFImageMimeType();
QString ImekaIO_EXPORT GetVFFImageFilter();

std::string ImekaIO_EXPORT GetRGBNiftiImageName();
std::string ImekaIO_EXPORT GetRGBNiftiImageDescription();
mitk::CustomMimeType ImekaIO_EXPORT GetRGBNiftiImageMimeType();
QString ImekaIO_EXPORT GetRGBNiftiImageFilter();

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_MIME_TYPE_HPP_INCLUDED
