
#include "Matrix3DProperty.hpp"

#include "PropertySerializer.hpp"

// We need a custom implementation because the macro mitkDefineGenericProperty 
// generates ToJSON/FromJSON that try to serialize Matrix3D directly to JSON,
// but nlohmann::json doesn't have a serializer for mitk::Matrix<double, 3, 3>.
// We only use XML serialization anyway via Matrix3DPropertySerializer.hpp.

namespace mitk
{
  // Manually expand the macro without the problematic JSON code
  Matrix3DProperty::Matrix3DProperty() : Superclass(Matrix3D()) {}
  Matrix3DProperty::Matrix3DProperty(const Matrix3DProperty &other) : GenericProperty<Matrix3D>(other) {}
  Matrix3DProperty::Matrix3DProperty(Matrix3D x) : Superclass(x) {}
  
  itk::LightObject::Pointer Matrix3DProperty::InternalClone() const
  {
    itk::LightObject::Pointer result(new Self(*this));
    result->UnRegister();
    return result;
  }
  
  // Return false to indicate JSON serialization is not supported
  bool Matrix3DProperty::ToJSON(nlohmann::json&) const
  {
    return false;
  }
  
  bool Matrix3DProperty::FromJSON(const nlohmann::json&)
  {
    return false;
  }
}

MITK_REGISTER_SERIALIZER(Matrix3DPropertySerializer);
