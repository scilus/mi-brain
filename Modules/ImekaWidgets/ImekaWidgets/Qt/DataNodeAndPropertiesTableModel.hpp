
#ifndef IMEKA_DATANODE_AND_PROPERTIES_TABLE_MODEL_HPP_INCLUDED
#define IMEKA_DATANODE_AND_PROPERTIES_TABLE_MODEL_HPP_INCLUDED

#include <QmitkDataStorageTableModel.h>

#include "ImekaWidgetsExports.h"

namespace Imeka
{

namespace Widgets
{

class ImekaWidgets_EXPORT DataNodeAndPropertiesTableModel
  : public QmitkDataStorageTableModel
{
  Q_OBJECT

public:
  typedef QString RowName;
  typedef QString ColumnName;
  typedef QPair<RowName, ColumnName> PropertyIdentifier;

  DataNodeAndPropertiesTableModel(
    mitk::DataStorage::Pointer dataStorage,
    mitk::NodePredicateBase* predicate,
    QObject* parent,
    const QStringList displayList);

  virtual QVariant headerData(
    int section, Qt::Orientation orientation, int role) const override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
  virtual int columnCount(const QModelIndex &parent) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;

  /// Adds a node to this model.
  /// There are two constraints for nodes in this model:
  /// 1. If a predicate is set (not null) the node will be checked against it.
  /// 2. The node has to have a data object (no one wants to see empty nodes).
  /// Also adds event listeners to the node.
  virtual void AddNode(const mitk::DataNode* node) override;

  virtual void RemoveNode(const mitk::DataNode* node) override;

  /// \brief Called when a single property was changed.
  /// The function searches through the list of nodes in this model for the
  /// changed property. If the property was found a dataChanged signal is
  /// emitted forcing
  /// all observing views to request the data again.
  virtual void PropertyModified(
    const itk::Object *caller,
    const itk::EventObject &event) override;

  virtual bool setData(
    const QModelIndex &index, const QVariant &value, int role) override;

protected:
  mitk::BaseProperty* GetBaseProperty(const QModelIndex &index) const;

  typedef std::map<int, QString> PropertyMap;
  PropertyMap m_PropertyMap;

  typedef std::map<mitk::BaseProperty*, unsigned long> BasePropertyMap;
  typedef std::map<QString, BasePropertyMap> DisplayPropertyMap;
  DisplayPropertyMap m_DisplayedProperties;
};

} // namespace Widgets

} // namespace Imeka

#endif // IMEKA_DATANODE_AND_PROPERTIES_TABLE_MODEL_HPP_INCLUDED

