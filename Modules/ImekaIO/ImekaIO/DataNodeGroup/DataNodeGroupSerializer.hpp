
#ifndef IMEKA_IO_DATANODE_GROUP_SERIALIZER_HPP_INCLUDED
#define IMEKA_IO_DATANODE_GROUP_SERIALIZER_HPP_INCLUDED

#include <mitkBaseDataSerializer.h>

#include "ImekaIOExports.h"

namespace mitk
{

class ImekaIO_EXPORT DataNodeGroupSerializer : public mitk::BaseDataSerializer
{
public:
  mitkClassMacro(DataNodeGroupSerializer, BaseDataSerializer);
  itkFactorylessNewMacro(Self);
  itkCloneMacro(Self)

  virtual std::string Serialize() override;

protected:
  DataNodeGroupSerializer() {};
  ~DataNodeGroupSerializer() {};
};

} // namespace mitk

#endif // IMEKA_IO_DATANODE_GROUP_SERIALIZER_HPP_INCLUDED

