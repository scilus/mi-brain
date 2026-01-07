
#ifndef IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED
#define IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED

#include <mitkBasePropertySerializer.h>
#include <tinyxml2.h>

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

  virtual tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument& doc) override
  {
    if (const Matrix3DProperty* prop =
      dynamic_cast<const Matrix3DProperty*>(m_Property.GetPointer()))
    {
      tinyxml2::XMLElement* element = doc.NewElement("matrix");
      Matrix3D matrix = prop->GetValue();
      element->SetAttribute("M00", matrix(0, 0));
      element->SetAttribute("M10", matrix(1, 0));
      element->SetAttribute("M20", matrix(2, 0));
      element->SetAttribute("M01", matrix(0, 1));
      element->SetAttribute("M11", matrix(1, 1));
      element->SetAttribute("M21", matrix(2, 1));
      element->SetAttribute("M02", matrix(0, 2));
      element->SetAttribute("M12", matrix(1, 2));
      element->SetAttribute("M22", matrix(2, 2));

      return element;
    }
    return nullptr;
  }

  virtual BaseProperty::Pointer Deserialize(const tinyxml2::XMLElement* element) override
  {
    if (!element) { return nullptr; }

    Matrix3D m;
    if(element->QueryDoubleAttribute(
        "M00", &m(0,0)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M10", &m(1,0)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M20", &m(2,0)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M01", &m(0,1)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M11", &m(1,1)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M21", &m(2,1)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M02", &m(0,2)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M12", &m(1,2)) != tinyxml2::XML_SUCCESS) { return nullptr; }
    if(element->QueryDoubleAttribute(
        "M22", &m(2,2)) != tinyxml2::XML_SUCCESS) { return nullptr; }

    return Matrix3DProperty::New(m).GetPointer();
  }

protected:
  Matrix3DPropertySerializer() {}
  virtual ~Matrix3DPropertySerializer() {}
};

}

#endif // IMEKA_PROPERTY_MATRIX3D_PROPERTY_SERIALIZER_HPP_INCLUDED

