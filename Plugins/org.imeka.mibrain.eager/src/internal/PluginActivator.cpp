
#include "PluginActivator.hpp"

#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateProperty.h>
#include <QmitkNodeDescriptorManager.h>

#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"

ctkPluginContext* PluginActivator::m_Context = nullptr;

ctkPluginContext* PluginActivator::GetContext()
{
  return m_Context;
}

void PluginActivator::start(ctkPluginContext* context)
{
  m_Context = context;

  auto manager = QmitkNodeDescriptorManager::GetInstance();

  auto isROICategory = mitk::NodePredicateProperty::New(
    Imeka::Fiber::GroupNodes::CategoryPropertyName,
    mitk::StringProperty::New("ROIs"));
  auto isTractsCategory = mitk::NodePredicateProperty::New(
    Imeka::Fiber::GroupNodes::CategoryPropertyName,
    mitk::StringProperty::New("Tracts"));
  auto isFiberBundle = mitk::NodePredicateOr::New(
    mitk::NodePredicateDataType::New("FiberBundle"),
    mitk::NodePredicateDataType::New("FilteredFiberBundle"));
  auto isCategoryOrTracts = mitk::NodePredicateOr::New(
    isTractsCategory, isFiberBundle);

  manager->AddDescriptor(
    new QmitkNodeDescriptor("ROICategory1", "", isROICategory, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("ROICategory2", "", isROICategory, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("TractsCategory1", "", isTractsCategory, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("PeaksImage", "",
    Imeka::Fiber::GetMaximaPredicate(true), manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("TractsGroup",
    "", Imeka::Fiber::GetTractGroupPredicate(), manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("SelectionROI",
    "", Imeka::Fiber::GetROIPredicate(), manager));

  // 1 and 2 because I want to group and order some actions.
  manager->AddDescriptor(
    new QmitkNodeDescriptor("FiberBundle1", "", isFiberBundle, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("TractsCategory2", "", isTractsCategory, manager));
  manager->AddDescriptor(new QmitkNodeDescriptor(
    "TractsCategoryOrTracts", "", isCategoryOrTracts, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("FiberBundle2", "", isFiberBundle, manager));
  manager->AddDescriptor(
    new QmitkNodeDescriptor("FiberBundle3", "", isFiberBundle, manager));

  // Already defined in org.imeka.common.eager but redefining it puts it
  // at the end of the list.
  manager->AddDescriptor(
    new QmitkNodeDescriptor("DataLoadedFromDisk", "",
    mitk::NodePredicateProperty::New("full_path"), manager));
}

void PluginActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  m_Context = nullptr;
}
