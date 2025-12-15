
#include "Matrix3DProperty.hpp"

#include "PropertySerializer.hpp"

mitkDefineGenericProperty(Matrix3DProperty, Matrix3D, Matrix3D());

MITK_REGISTER_SERIALIZER(Matrix3DPropertySerializer);
