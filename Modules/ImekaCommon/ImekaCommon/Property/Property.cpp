
#include "Property.hpp"

namespace Imeka
{

namespace Property
{

const QString ImekaCommon_EXPORT GetKey(
  const QString &key, const unsigned int idx)
{
  return QString("%1.%2").arg(key).arg(idx);
}

bool ImekaCommon_EXPORT HasProperty(
  const mitk::DataNode* node, const char* key)
{
  return HasProperty(node, std::string(key));
}
bool ImekaCommon_EXPORT HasProperty(
  const mitk::DataNode* node, const std::string& key)
{
  return node->GetPropertyList()->GetMap()->count(key);
}
bool ImekaCommon_EXPORT HasProperty(
  const mitk::DataNode* node, const QString& key)
{
  return HasProperty(node, key.toStdString());
}

// This function must be used for string type.
const QString ImekaCommon_EXPORT GetString(
  mitk::DataNode* node, const QString &key)
{
  std::string str = "";
  node->GetStringProperty(key.toStdString().c_str(), str);
  return QString::fromStdString(str);
}

// This function must be used for string type.
const QString ImekaCommon_EXPORT GetString(
  mitk::DataNode::Pointer node,
  const QString &key,
  const unsigned int idx)
{
  std::string str = "";
  node->GetStringProperty(GetKey(key, idx).toStdString().c_str(), str);
  return QString::fromStdString(str);
}

} // namespace Property

} // namespace Imeka

