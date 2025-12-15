
#ifndef IMEKA_COMMON_PERSISTENCE_HPP_INCLUDED
#define IMEKA_COMMON_PERSISTENCE_HPP_INCLUDED

#include <mitkIPersistenceService.h>

#include "ImekaCommonExports.h"

namespace Imeka
{

class ImekaCommon_EXPORT Persistence
{
public:
  Persistence(const std::string&);
  ~Persistence() {}

  mitk::PropertyList* GetProperties();

  void AddToObservers(mitk::PropertyListReplacedObserver*);
  void RemoveFromObservers(mitk::PropertyListReplacedObserver*);

private:
  PERSISTENCE_GET_SERVICE_METHOD_MACRO

  const std::string m_Id;
};

} // namespace Imeka

#endif // IMEKA_COMMON_PERSISTENCE_HPP_INCLUDED
