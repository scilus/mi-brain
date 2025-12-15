
#ifndef IMEKA_COMMON_PREDICATE_HPP_INCLUDED
#define IMEKA_COMMON_PREDICATE_HPP_INCLUDED

#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateProperty.h>

#include "mitkNodePredicateImageInfo.hpp"

#include "ImekaCommonExports.h"

namespace mitk
{
  class DataStorage;
  class NodePredicateCompositeBase;
}

namespace Imeka
{

namespace Predicate
{

mitk::NodePredicateImageInfo::Pointer ImekaCommon_EXPORT GetRGBPredicate();

bool ImekaCommon_EXPORT AtLeastOne(
  const std::string&, mitk::DataStorage*,
  const bool = false, const bool = false);

mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char*, const bool);
mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char*, const char*);
mitk::NodePredicateProperty::Pointer ImekaCommon_EXPORT Property(
  const char*, const std::string&);

template <class T>
mitk::NodePredicateNot::Pointer NotProperty(
  const char* propertyName, const T t)
{
  return mitk::NodePredicateNot::New(Property(propertyName, t));
}

mitk::NodePredicateNot::Pointer ImekaCommon_EXPORT NotHelperObject();

mitk::NodePredicateBase::Pointer ImekaCommon_EXPORT
  GetPredicateFor(const std::string& dataType);
mitk::NodePredicateOr::Pointer ImekaCommon_EXPORT
IsBoundingObjectPredicate();

template <class T, class IT>
void Fill(
  mitk::NodePredicateCompositeBase* predicate,
  const IT &begin, const IT &end)
{
  for (IT it = begin; it != end; ++it)
  {
    predicate->AddPredicate(T::New(it->c_str()));
  }
}

template <class T, class Container>
void Fill(mitk::NodePredicateCompositeBase* predicate, const Container &c)
{
  Fill<T>(predicate, begin(c), end(c));
}

} // namespace Predicate

} // namespace Imeka

#endif // IMEKA_COMMON_PREDICATE_HPP_INCLUDED

