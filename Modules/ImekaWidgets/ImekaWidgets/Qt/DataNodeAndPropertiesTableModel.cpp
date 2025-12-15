
#include "DataNodeAndPropertiesTableModel.hpp"

#include <QStringList>

#include "ImekaCommon/Property/BasePropertyUtils.hpp"

namespace Imeka
{

namespace Widgets
{

DataNodeAndPropertiesTableModel::DataNodeAndPropertiesTableModel(
  mitk::DataStorage::Pointer dataStorage,
  mitk::NodePredicateBase* predicate,
  QObject* parent,
  const QStringList displayList)
  : QmitkDataStorageTableModel(dataStorage, predicate, parent)
{
  unsigned int idx = 0;
  for (const auto& str: displayList)
  {
    m_PropertyMap[idx++] = str.toLower();
  }
}

QVariant DataNodeAndPropertiesTableModel::headerData(
  int section, Qt::Orientation orientation, int role) const
{
  QVariant headerData;

  // show only horizontal header
  if (role == Qt::DisplayRole)
  {
    if (orientation == Qt::Horizontal)
    {
      QString propertyName = m_PropertyMap.find(section)->second;
      propertyName[0] = propertyName[0].toUpper();
      headerData = propertyName;
    }
    else if (orientation == Qt::Vertical)
    {
      headerData = section + 1;
    }
  }

  return headerData;
}

Qt::ItemFlags DataNodeAndPropertiesTableModel::flags(
  const QModelIndex &index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if (m_PropertyMap.find(index.column())->second.compare("color") == 0)
  {
    flags &= ~Qt::ItemIsSelectable;
  }
  return flags;
}

int DataNodeAndPropertiesTableModel::columnCount(
  const QModelIndex &) const
{
  return m_PropertyMap.size();
}


mitk::BaseProperty* DataNodeAndPropertiesTableModel::GetBaseProperty(
  const QModelIndex &index) const
{
  const QString propertyName = m_PropertyMap.find(index.column())->second;
  mitk::BaseProperty* propertyValue;
  GetNode(index)->GetProperty(
    propertyValue, propertyName.toStdString().c_str());
  return propertyValue;
}

QVariant DataNodeAndPropertiesTableModel::data(
  const QModelIndex &index, int role) const
{
  if (!index.isValid()
    || (role != Qt::DisplayRole && role != Qt::BackgroundColorRole)
    || (role == Qt::DisplayRole
        && m_PropertyMap.find(index.column())->second.compare("color") == 0)
    )
  {
    return QVariant();
  }

  mitk::BaseProperty* prop = GetBaseProperty(index);
  return Imeka::Common::BasePropertyToQVariant(prop);
}

void DataNodeAndPropertiesTableModel::AddNode(const mitk::DataNode* node)
{
  // garantee no recursions when a new node event is thrown
  if (m_BlockEvents) { return; }
  if (m_Predicate.IsNotNull() && !m_Predicate->CheckNode(node)) { return; }
  if (node->GetData() == 0) { return; }

  // MITK can call AddNode 2 times...
  auto it = std::find(std::begin(m_NodeSet), std::end(m_NodeSet), node);
  if (it != m_NodeSet.end()) { return; }

  auto propertyModifiedCommand =
    itk::MemberCommand<QmitkDataStorageTableModel>::New();
  propertyModifiedCommand->SetCallbackFunction(
    this, &QmitkDataStorageTableModel::PropertyModified);

  for (const auto& keyValue: m_PropertyMap)
  {
    auto propertyName = keyValue.second.toStdString().c_str();
    auto tempProperty = node->GetProperty(propertyName);
    if (tempProperty)
    {
      m_DisplayedProperties[propertyName][tempProperty] =
        tempProperty->AddObserver(
          itk::ModifiedEvent(), propertyModifiedCommand);
    }
  }

  beginInsertRows(QModelIndex(), m_NodeSet.size(), m_NodeSet.size());
  m_NodeSet.push_back(const_cast<mitk::DataNode*>(node));
  endInsertRows();
}

void DataNodeAndPropertiesTableModel::RemoveNode(const mitk::DataNode* node)
{
  if (m_BlockEvents) { return; }

  auto nodeIt = std::find(m_NodeSet.begin(), m_NodeSet.end(), node);
  if (nodeIt == m_NodeSet.end()) { return; }

  // now: remove listeners for name property ...
  for (const auto& keyValue: m_PropertyMap)
  {
    const auto propertyName = keyValue.second.toUtf8().constData();
    auto tempProperty = (*nodeIt)->GetProperty(propertyName);
    if (!tempProperty) { continue; }

    DisplayPropertyMap::iterator displayProp =
      m_DisplayedProperties.find(propertyName);
    if (displayProp != m_DisplayedProperties.end()
     && displayProp->second.find(tempProperty) != displayProp->second.end())
    {
      tempProperty->RemoveObserver(displayProp->second
          .find(tempProperty)->second);
    }
  }

  const unsigned int row = std::distance(m_NodeSet.begin(), nodeIt);

  // (QModelIndex is empty because we dont have a tree model)
  this->beginRemoveRows(QModelIndex(), row, row);
  m_NodeSet.erase(nodeIt);
  endRemoveRows();
}

void DataNodeAndPropertiesTableModel::PropertyModified(
  const itk::Object *caller, const itk::EventObject &)
{
  if (m_BlockEvents) { return; }

  auto modifiedProperty = dynamic_cast<const mitk::BaseProperty*>(caller);
  if (!modifiedProperty) { return; }

  int column = 0, row = 0;
  auto findProperty = [&column, &row, this](const mitk::BaseProperty* prop){
    for (const auto node: m_NodeSet)
    {
      for (const auto& keyValue: m_PropertyMap)
      {
        auto propertyName = keyValue.second.toStdString().c_str();
        auto tempProperty = node->GetProperty(propertyName);
        if (prop == tempProperty)
        {
          column = keyValue.first;
          return;
        }
      }
      ++row;
    }
    column = -1;
    row = -1;
  };

  findProperty(modifiedProperty);
  const QModelIndex indexOfChangedProperty = index(row, column);
  emit dataChanged(indexOfChangedProperty, indexOfChangedProperty);
}

bool DataNodeAndPropertiesTableModel::setData(
  const QModelIndex&, const QVariant&, int)
{
  // Data editing disabled
  return true;
}

} // namespace Widgets

} // namespace Imeka
