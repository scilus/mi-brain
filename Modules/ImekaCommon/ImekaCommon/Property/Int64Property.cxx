
#include "Int64Property.hpp"

#include "PropertySerializer.hpp"

mitkDefineGenericProperty(Int64Property, long long, 0L);

MITK_REGISTER_SERIALIZER(Int64PropertySerializer);
