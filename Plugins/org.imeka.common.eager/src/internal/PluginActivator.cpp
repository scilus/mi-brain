#include "PluginActivator.hpp"

#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateProperty.h>
#include <QmitkNodeDescriptorManager.h>

#include "ImekaCommon/Predicate.hpp"

ctkPluginContext* PluginActivator::m_Context = nullptr;

ctkPluginContext* PluginActivator::GetContext()
{
  return m_Context;
}

void PluginActivator::start(ctkPluginContext* context)
{
  m_Context = context;

  auto manager = QmitkNodeDescriptorManager::GetInstance();

  manager->AddDescriptor(
    new QmitkNodeDescriptor(tr("Surface"), "",
    mitk::NodePredicateDataType::New("Surface"), manager));

  manager->AddDescriptor(
    new QmitkNodeDescriptor(tr("RGBImage"), "",
    Imeka::Predicate::GetRGBPredicate(), manager));

  manager->AddDescriptor(new QmitkNodeDescriptor(
    tr("BoundingObject"), "",
    Imeka::Predicate::IsBoundingObjectPredicate(), manager));

  manager->AddDescriptor(new QmitkNodeDescriptor(
    tr("DataLoadedFromDisk"), "",
    mitk::NodePredicateProperty::New("full_path"), manager));
}

void PluginActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  m_Context = nullptr;
}
