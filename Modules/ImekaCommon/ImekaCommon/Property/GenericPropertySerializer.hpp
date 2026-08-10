
#ifndef IMEKA_GENERIC_PROPERTY_SERIALIZER_HPP_INCLUDED
#define IMEKA_GENERIC_PROPERTY_SERIALIZER_HPP_INCLUDED

#include <boost/lexical_cast.hpp>

#include <tinyxml2.h>
#include <mitkBasePropertySerializer.h>
#include <mitkGenericProperty.h>

#include "ImekaCommonExports.h"

namespace mitk
{

/* This could be contributed to MITK. It's a single class that replaces all
   generic property serializer: Bool, USHort, UInt, Int, Float, Double, String.
   It can't do the LookupTables, Points and Vectors though.
*/
template <class Property>
class ImekaCommon_EXPORT GenericPropertySerializer
  : public BasePropertySerializer
{
  typedef typename Property::ValueType T;

public:
  mitkClassMacro(GenericPropertySerializer, BasePropertySerializer);
  itkNewMacro(Self);

  virtual tinyxml2::XMLElement* Serialize(tinyxml2::XMLDocument& doc) override
  {
    const GenericProperty<T>* property =
      dynamic_cast<const GenericProperty<T>*>(m_Property.GetPointer());
    if (property)
    {
      tinyxml2::XMLElement* element = doc.NewElement("Generic");
      element->SetAttribute("value", property->GetValueAsString().c_str());
      return element;
    }
    return nullptr;
  }

  virtual BaseProperty::Pointer Deserialize(const tinyxml2::XMLElement* element) override
  {
    if (!element) { return nullptr; }

    const char* strVal = element->Attribute("value");
    if (strVal)
    {
      const T val = boost::lexical_cast<T>(std::string(strVal));
      return Property::New(val).GetPointer();
    }
    return nullptr;
  }

protected:
  GenericPropertySerializer() {}
  virtual ~GenericPropertySerializer() {}
};

} // namespace mitk

#endif // IMEKA_GENERIC_PROPERTY_SERIALIZER_HPP_INCLUDED

