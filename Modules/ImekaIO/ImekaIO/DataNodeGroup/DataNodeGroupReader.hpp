
#ifndef IMEKA_IO_DATANODE_GROUP_READER_HPP_INCLUDED
#define IMEKA_IO_DATANODE_GROUP_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>

#include "ImekaIOExports.h"

namespace Imeka
{

namespace IO
{

class ImekaIO_EXPORT DataNodeGroupReader : public mitk::AbstractFileReader
{
public:
  DataNodeGroupReader();
  virtual ~DataNodeGroupReader(){}
  DataNodeGroupReader(const DataNodeGroupReader& other);
  virtual DataNodeGroupReader* Clone() const;

  using mitk::AbstractFileReader::Read;

protected:
  virtual std::vector<itk::SmartPointer<mitk::BaseData>> DoRead();

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_DATANODE_GROUP_READER_HPP_INCLUDED

