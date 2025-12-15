
#ifndef IMEKA_NODE_TABLE_VIEW_UTILS_HPP_INCLUDED
#define IMEKA_NODE_TABLE_VIEW_UTILS_HPP_INCLUDED

#include "ImekaCommon/types.hpp"
#include <ImekaWidgets/Qt/DataNodeAndPropertiesTableModel.hpp>

#include <QTableView>

#include "ImekaWidgetsExports.h"

namespace Imeka
{

namespace Widgets
{

/*
  To be called in a SelectionChanged SLOT, update @selectedItems so that it
  always contains the selected nodes of @tableView.

  IMO this is a complicated way to obtain the selected nodes. Maybe calling
  QmitkDataStorageTableModel::GetNode on each row of
  tableView->selectionModel->selectedRows would be simpler.
 */
void ImekaWidgets_EXPORT UpdateSelected(
  const QTableView* tableView,
  const QItemSelection& selected,
  const QItemSelection& deselected,
  Nodes& nodes);

Nodes ImekaWidgets_EXPORT IndicesToNodes(
  const QTableView* tableView,
  const QItemSelection& indices);

} // namespace Widgets

} // namespace Imeka

#endif // IMEKA_NODE_TABLE_VIEW_UTILS_HPP_INCLUDED
