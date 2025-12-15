#ifndef IMEKA_TCK_WRITER_HPP_INCLUDED
#define IMEKA_TCK_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

#include <vtkPolyDataWriter.h>

namespace mitk
{

class FiberBundleTckWriter : public mitk::AbstractFileWriter
{
public:
  FiberBundleTckWriter();
  FiberBundleTckWriter(const FiberBundleTckWriter & other);
  virtual FiberBundleTckWriter* Clone() const override;
  virtual ~FiberBundleTckWriter() {}

  using mitk::AbstractFileWriter::Write;
  virtual void Write() override;
};

} // namespace mitk

#endif // IMEKA_TCK_WRITER_HPP_INCLUDED
