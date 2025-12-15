#ifndef IMEKA_TCK_READER_HPP_INCLUDED
#define IMEKA_TCK_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>
#include <mitkCommon.h>
#include <mitkFileReader.h>

#include <vtkSmartPointer.h>

namespace mitk
{

class FiberBundleTckReader : public AbstractFileReader
{
public:
  FiberBundleTckReader();
  virtual ~FiberBundleTckReader(){}
  FiberBundleTckReader(const FiberBundleTckReader& other);
  virtual FiberBundleTckReader * Clone() const override;

  using mitk::AbstractFileReader::Read;
  virtual std::vector<itk::SmartPointer<BaseData> > Read() override;

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;
};

} // namespace mitk

#endif // IMEKA_TCK_READER_HPP_INCLUDED
