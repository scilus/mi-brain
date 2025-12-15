
#ifndef IMEKA_DICOM_WRITER_HPP_INCLUDED
#define IMEKA_DICOM_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

#include "ImekaIOExports.h"

namespace Imeka
{

namespace IO
{

class DicomWriter : public mitk::AbstractFileWriter
{
public:
  DicomWriter();
  DicomWriter(const DicomWriter& other);

  virtual DicomWriter* Clone() const;
  virtual ~DicomWriter() {}

  using mitk::AbstractFileWriter::Write;
  virtual void Write();
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_DICOM_WRITER_HPP_INCLUDED
