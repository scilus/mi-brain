
#ifndef IMEKA_COMMON_BASE_PROPERTY_UTILS_HPP_INCLUDED
#define IMEKA_COMMON_BASE_PROPERTY_UTILS_HPP_INCLUDED

#include <mitkPropertyList.h>

#include <QVariant>

#include "ImekaCommonExports.h"

namespace Imeka
{

namespace Common
{

QVariant ImekaCommon_EXPORT BasePropertyToQVariant(
  mitk::BaseProperty* baseProp);

// Perform operation between two property lists. The property type and name
// need to be the same.
template<typename PropertyType, typename BinaryFunction>
mitk::PropertyList::Pointer PropertyListApply(
  const mitk::PropertyList* list1,
  const mitk::PropertyList* list2,
  BinaryFunction func)
{
  // We don't care on which map we iterate. We only perform the operation on
  // similar properties anyway
  auto retList = mitk::PropertyList::New();
  for (const auto& p : *list1->GetMap())
  {
    if (const auto prop1 = dynamic_cast<PropertyType*>(p.second.GetPointer()))
    {
      const typename BinaryFunction::result_type val1 = prop1->GetValue();
      typename BinaryFunction::result_type val2;
      if (list2->GetPropertyValue(p.first.c_str(), val2))
      {
        const typename BinaryFunction::result_type res = func(val1, val2);
        retList->SetProperty(p.first, PropertyType::New(res));
      }
    }
  }

  return retList;
}

} // namespace Common

} // namespace Imeka

#endif // IMEKA_COMMON_BASE_PROPERTY_UTILS_HPP_INCLUDED

