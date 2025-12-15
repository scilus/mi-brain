
#ifndef IMEKA_PROPERTY_PROPERTY_HPP_INCLUDED
#define IMEKA_PROPERTY_PROPERTY_HPP_INCLUDED

#include <mitkDataNode.h>
#include <mitkProperties.h>

#include <QString>

#include "Matrix3DProperty.hpp"

#include "ImekaCommonExports.h"

namespace Imeka
{

namespace Property
{

inline mitk::BoolProperty::Pointer New(const bool b)
{
  return mitk::BoolProperty::New(b);
}
inline mitk::IntProperty::Pointer New(const int i)
{
  return mitk::IntProperty::New(i);
}
inline mitk::UIntProperty::Pointer New(const unsigned int i)
{
  return mitk::UIntProperty::New(i);
}
inline mitk::UIntProperty::Pointer New(const size_t i)
{
  return mitk::UIntProperty::New(i);
}
inline mitk::StringProperty::Pointer New(const char* s)
{
  return mitk::StringProperty::New(s);
}
inline mitk::DoubleProperty::Pointer New(const double& d)
{
  return mitk::DoubleProperty::New(d);
}
inline mitk::Vector3DProperty::Pointer New(const mitk::Vector3D& v)
{
  return mitk::Vector3DProperty::New(v);
}
inline mitk::Matrix3DProperty::Pointer New(const mitk::Matrix3D& m)
{
  return mitk::Matrix3DProperty::New(m);
}

const QString ImekaCommon_EXPORT
GetKey(const QString &key, const unsigned int idx);

bool ImekaCommon_EXPORT
HasProperty(const mitk::DataNode* node, const char* key);
bool ImekaCommon_EXPORT
HasProperty(const mitk::DataNode* node, const std::string& key);
bool ImekaCommon_EXPORT
HasProperty(const mitk::DataNode* node, const QString& key);

template <class T>
const T Get(
  const mitk::DataNode* node,
  const QString &key,
  const T defaultValue)
{
  T var = defaultValue;
  node->GetPropertyValue(key.toStdString().c_str(), var);
  return var;
}

template <class T>
const T Get(
  const mitk::PropertyList* propertyList,
  const QString &key,
  const T defaultValue)
{
  T var = defaultValue;
  propertyList->GetPropertyValue(key.toStdString().c_str(), var);
  return var;
}

// This function must be used for string type.
const QString ImekaCommon_EXPORT
GetString(mitk::DataNode* node, const QString &key);

template <class T>
const T Get(
  const mitk::DataNode* node,
  const QString &key,
  const unsigned int idx,
  const T defaultValue)
{
  T var = defaultValue;
  const std::string realKey = GetKey(key, idx).toStdString();
  node->GetPropertyValue(realKey.c_str(), var);
  return var;
}

// This function must be used for string type.
const QString ImekaCommon_EXPORT
GetString(
  mitk::DataNode* node,
  const QString &key,
  const unsigned int idx);

template <class T>
void Set(mitk::DataNode* node, const char* key, const T value)
{
  auto prop = New(value);
  node->SetProperty(key, prop);
}

template <class T>
void Set(mitk::DataNode* node, const QString &key, const T value)
{
  auto prop = New(value);
  node->SetProperty(key.toStdString().c_str(), prop);
}

template <class T>
void Set(
  mitk::DataNode* node,
  const QString &key, const unsigned int idx,
  const T value)
{
  auto prop = New(value);
  node->SetProperty(GetKey(key, idx).toStdString().c_str(), prop);
}
} // namespace Property

} // namespace Imeka

#endif //IMEKA_PROPERTY_PROPERTY_HPP_INCLUDED

