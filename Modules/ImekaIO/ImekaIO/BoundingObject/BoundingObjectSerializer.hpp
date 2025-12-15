
#ifndef IMEKA_BOUNDING_OBJECT_SERIALIZER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_SERIALIZER_HPP_INCLUDED

#include "mitkBaseDataSerializer.h"

#include "ImekaIOExports.h"

namespace mitk
{

class ImekaIO_EXPORT BoundingObjectSerializer : public BaseDataSerializer
{
public:
  mitkClassMacro(BoundingObjectSerializer, BaseDataSerializer);
  itkFactorylessNewMacro(Self)
  itkCloneMacro(Self)

  virtual std::string Serialize() override;

protected:
  BoundingObjectSerializer() {};
  virtual ~BoundingObjectSerializer() {};
};

typedef BoundingObjectSerializer CuboidSerializer;
typedef BoundingObjectSerializer EllipsoidSerializer;
typedef BoundingObjectSerializer ConeSerializer;
typedef BoundingObjectSerializer CylinderSerializer;

} // namespace mitk

#endif // IMEKA_BOUNDING_OBJECT_SERIALIZER_HPP_INCLUDED

