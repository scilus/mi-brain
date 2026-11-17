
#ifndef IMEKA_IO_MIME_TYPE_HPP_INCLUDED
#define IMEKA_IO_MIME_TYPE_HPP_INCLUDED

#include <mitkCustomMimeType.h>

#include <QString>

#include "ImekaIOExports.h"

namespace Imeka
{

namespace IO
{

std::string IMEKAIO_EXPORT GetBoundingObjectName();
std::string IMEKAIO_EXPORT GetBoundingObjectDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetBoundingObjectMimeType();
QString IMEKAIO_EXPORT GetBoundingObjectFilter();

std::string IMEKAIO_EXPORT GetDataNodeGroupName();
std::string IMEKAIO_EXPORT GetDataNodeGroupDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetDataNodeGroupMimeType();
QString IMEKAIO_EXPORT GetDataNodeGroupFilter();

std::string IMEKAIO_EXPORT GetDicomGroupName();
std::string IMEKAIO_EXPORT GetDicomDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetDicomMimeType();
QString IMEKAIO_EXPORT GetDicomFilter();

std::string IMEKAIO_EXPORT GetFDFImageName();
std::string IMEKAIO_EXPORT GetFDFImageDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetFDFImageMimeType();
QString IMEKAIO_EXPORT GetFDFImageFilter();

std::string IMEKAIO_EXPORT GetVFFImageName();
std::string IMEKAIO_EXPORT GetVFFImageDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetVFFImageMimeType();
QString IMEKAIO_EXPORT GetVFFImageFilter();

std::string IMEKAIO_EXPORT GetRGBNiftiImageName();
std::string IMEKAIO_EXPORT GetRGBNiftiImageDescription();
mitk::CustomMimeType IMEKAIO_EXPORT GetRGBNiftiImageMimeType();
QString IMEKAIO_EXPORT GetRGBNiftiImageFilter();

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_MIME_TYPE_HPP_INCLUDED
