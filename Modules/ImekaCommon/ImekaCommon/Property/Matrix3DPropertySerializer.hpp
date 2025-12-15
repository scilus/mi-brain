
#ifndef IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED
#define IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED

#include <mitkBasePropertySerializer.h>

#include "Matrix3DProperty.hpp"

#include "ImekaCommonExports.h"

namespace mitk
{

class ImekaCommon_EXPORT Matrix3DPropertySerializer
  : public BasePropertySerializer
{
public:
  mitkClassMacro(Matrix3DPropertySerializer, BasePropertySerializer);
  itkNewMacro(Self);

  virtual TiXmlElement* Serialize() override
  {
    if (const Matrix3DProperty* prop =
      dynamic_cast<const Matrix3DProperty*>(m_Property.GetPointer()))
    {
      TiXmlElement* element = new TiXmlElement("matrix");
      Matrix3D matrix = prop->GetValue();
      element->SetDoubleAttribute("M00", matrix(0, 0));
      element->SetDoubleAttribute("M10", matrix(1, 0));
      element->SetDoubleAttribute("M20", matrix(2, 0));
      element->SetDoubleAttribute("M01", matrix(0, 1));
      element->SetDoubleAttribute("M11", matrix(1, 1));
      element->SetDoubleAttribute("M21", matrix(2, 1));
      element->SetDoubleAttribute("M02", matrix(0, 2));
      element->SetDoubleAttribute("M12", matrix(1, 2));
      element->SetDoubleAttribute("M22", matrix(2, 2));

      return element;
    }
    return nullptr;
  }

  virtual BaseProperty::Pointer Deserialize(TiXmlElement* element) override
  {
    if (!element) { return nullptr; }

    Matrix3D m;
    if(element->QueryDoubleAttribute(
        "M00", &m(0,0)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M10", &m(1,0)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M20", &m(2,0)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M01", &m(0,1)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M11", &m(1,1)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M21", &m(2,1)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M02", &m(0,2)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M12", &m(1,2)) != TIXML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M22", &m(2,2)) != TIXML_SUCCESS) { return nullptr; }

    return Matrix3DProperty::New(m).GetPointer();
  }

protected:
  Matrix3DPropertySerializer() {}
  virtual ~Matrix3DPropertySerializer() {}
};

}

#endif // IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED

