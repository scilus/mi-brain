
#include "NodeTableViewUtils.hpp"

namespace Imeka
{

namespace Widgets
{

void UpdateSelected(
  const QTableView* tableView,
  const QItemSelection& selected,
  const QItemSelection& deselected,
  Nodes& nodes)
{
  const auto masksModel =
    static_cast<DataNodeAndPropertiesTableModel*>(tableView->model());

  for (const auto& index : deselected.indexes())
  {
    const auto node = masksModel->GetNode(index);
    nodes.erase(std::find(std::begin(nodes), std::end(nodes), node));
  }

  for (const auto& index : selected.indexes())
  {
    const auto node = masksModel->GetNode(index);
    nodes.push_back(node);
  }
}

Nodes IndicesToNodes(
  const QTableView* tableView,
  const QItemSelection& indices)
{
  const auto masksModel =
    static_cast<DataNodeAndPropertiesTableModel*>(tableView->model());
  Nodes nodes;
  nodes.reserve(indices.size());
  for (const auto& index : indices.indexes())
  {
    const auto node = masksModel->GetNode(index);
    nodes.push_back(node);
  }
  return nodes;
}

} // namespace Widgets

} // namespace Imeka

