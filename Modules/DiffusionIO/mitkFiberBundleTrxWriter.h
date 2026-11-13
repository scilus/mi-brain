#ifndef IMEKA_TRX_WRITER_HPP_INCLUDED
#define IMEKA_TRX_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

namespace mitk
{

class FiberBundleTrxWriter : public mitk::AbstractFileWriter
{
public:
  FiberBundleTrxWriter();
  FiberBundleTrxWriter(const FiberBundleTrxWriter & other);
  virtual FiberBundleTrxWriter* Clone() const override;
  virtual ~FiberBundleTrxWriter() {}

  using mitk::AbstractFileWriter::Write;
  virtual void Write() override;
};

} // namespace mitk

#endif // IMEKA_TRX_WRITER_HPP_INCLUDED
