
#ifndef IMEKA_IO_RGB_NIFTI_FILE_READER_HPP_INCLUDED
#define IMEKA_IO_RGB_NIFTI_FILE_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>
#include <mitkCustomMimeType.h>
#include <mitkLogMacros.h>

#include "ImekaIOExports.h"

namespace Imeka
{
namespace IO{

class ImekaIO_EXPORT RGBNiftiFileReader : public mitk::AbstractFileReader
{
public:
  RGBNiftiFileReader();
  RGBNiftiFileReader(const RGBNiftiFileReader& other);

  virtual ~RGBNiftiFileReader() override = default;
  virtual RGBNiftiFileReader* Clone() const override;
  virtual std::vector<mitk::BaseData::Pointer> DoRead() override;

  ConfidenceLevel GetConfidenceLevel() const override;
private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;
};
} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_RGB_NIFTI_FILE_READER_HPP_INCLUDED