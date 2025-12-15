
#include "Persistence.hpp"

namespace Imeka
{

Persistence::Persistence(const std::string& id)
  : m_Id(id)
{}

mitk::PropertyList* Persistence::GetProperties()
{
  std::string id(m_Id); // Can't be a tamporary
  return GetPersistenceService()->GetPropertyList(id);
}

void Persistence::AddToObservers(
  mitk::PropertyListReplacedObserver* view)
{
  GetPersistenceService()->AddPropertyListReplacedObserver(view);
}

void Persistence::RemoveFromObservers(
  mitk::PropertyListReplacedObserver* view)
{
  GetPersistenceService()->RemovePropertyListReplacedObserver(view);
}

} // namespace Imeka
