
#include "Predicate.hpp"

#include <mitkDataStorage.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateAnd.h>
#include <mitkProperties.h>

namespace Imeka
{

namespace Predicate
{

mitk::NodePredicateImageInfo::Pointer ImekaCommon_EXPORT GetRGBPredicate()
{
  auto pred = mitk::NodePredicateImageInfo::New();
  pred->SetPixelComponents(1);
  pred->SetDimension(4);
  pred->AddDimensionSize(3, 3);
  pred->SetPixelType("unsigned_char");
  return pred;
}

bool ImekaCommon_EXPORT AtLeastOne(
  const std::string& dataType, mitk::DataStorage* dataStorage,
  const bool dropIfHidden, const bool dropIfHelperObject)
{
  auto dataTypePred = GetPredicateFor(dataType);
  if (!dropIfHelperObject && !dropIfHidden)
  {
    return dataStorage->GetNode(dataTypePred) != nullptr;
  }

  mitk::NodePredicateAnd::Pointer pred = nullptr;
  if (dropIfHidden)
  {
    pred = mitk::NodePredicateAnd::New(
      dataTypePred, Property("visible", true));
    if (dropIfHelperObject)
    {
      pred = mitk::NodePredicateAnd::New(pred, NotHelperObject());
    }
  }
  else
  {
    pred = mitk::NodePredicateAnd::New(dataTypePred, NotHelperObject());
  }

  return dataStorage->GetNode(pred) != nullptr;
}

mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char* propertyName, const bool b)
{
  return mitk::NodePredicateProperty::New(
    propertyName, mitk::BoolProperty::New(b));
}

mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char* propertyName, const char* c)
{
  return mitk::NodePredicateProperty::New(
    propertyName, mitk::StringProperty::New(c));
}

mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char* propertyName, const std::string& str)
{
  return Property(propertyName, str.c_str());
}

mitk::NodePredicateNot::Pointer ImekaCommon_EXPORT NotHelperObject()
{
  return NotProperty("helper object", true);
}

mitk::NodePredicateBase::Pointer ImekaCommon_EXPORT
GetPredicateFor(const std::string& dataType)
{
  if (dataType == "BoundingObject" || dataType == "SelectionObject")
  {
    return mitk::NodePredicateBase::Pointer(IsBoundingObjectPredicate());
  }
  return mitk::NodePredicateBase::Pointer(
    mitk::NodePredicateDataType::New(dataType.c_str()));
}

mitk::NodePredicateOr::Pointer IsBoundingObjectPredicate()
{
  auto isBoundingObject = mitk::NodePredicateOr::New();
  Imeka::Predicate::Fill<mitk::NodePredicateDataType>(
    isBoundingObject,
    std::vector<std::string>{"Cuboid", "Ellipsoid", "Cone", "Cylinder"});
  return isBoundingObject;
}

} // namespace Predicate

} // namespace Imeka

