
#include "UInt64Property.hpp"

#include "PropertySerializer.hpp"

mitkDefineGenericProperty(UInt64Property, unsigned long long, 0UL);

MITK_REGISTER_SERIALIZER(UInt64PropertySerializer);
