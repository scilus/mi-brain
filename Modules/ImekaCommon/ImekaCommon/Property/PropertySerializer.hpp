
#ifndef IMEKA_PROPERTY_SERIALIZER_HPP_INCLUDED
#define IMEKA_PROPERTY_SERIALIZER_HPP_INCLUDED

#include "GenericPropertySerializer.hpp"
#include "Matrix3DPropertySerializer.hpp"

#include "Int64Property.hpp"
#include "UInt64Property.hpp"

namespace mitk
{
  // Imeka defines only 3 new types of property: Int64, Uint64 and Matrix3D.
  // Matrix3D serializer is defined in Matrix3DPropertySerializer.hpp.
  typedef GenericPropertySerializer<Int64Property> Int64PropertySerializer;
  typedef GenericPropertySerializer<UInt64Property> UInt64PropertySerializer;
}

#endif // IMEKA_PROPERTY_SERIALIZER_HPP_INCLUDED
