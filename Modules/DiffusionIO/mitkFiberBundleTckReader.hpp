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
  FiberBundleTckReader* Clone() const override;

  std::vector<itk::SmartPointer<BaseData>> DoRead() override;

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;
};

} // namespace mitk

#endif // IMEKA_TCK_READER_HPP_INCLUDED
