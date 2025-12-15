
#include "SaveVisibleFibersAbstractAction.hpp"

#include <boost/range/join.hpp>

#include <QApplication>
#include <QMessageBox>

#include "ImekaCommon/types.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"

void SaveVisibleFibersAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  /* If we use the icons in the DM, it's impossible to select more than one
     node because selectedNodes is modified just before calling this method,
     so almost all the code here is useless. However, the user can select many
     nodes, right click and Save, which would make it possible to:
     - Have nodes that aren't Category, fiber bundle or TG.
     - Have selected node(s) that are parents of other selected nodes. */

  // Category, fiber bundle or TG
  Nodes cNodes, fbNodes, tgNodes;
  for (auto node : selectedNodes)
  {
    if (Imeka::Fiber::IsTract(node))
    {
      fbNodes.push_back(node);
    }
    else if (Imeka::Fiber::IsTractsCategory(node))
    {
      cNodes.push_back(node);
    }
    else if (Imeka::Fiber::IsTractGroup(node))
    {
      tgNodes.push_back(node);
    }
  }

  // Can't mix any of them because what we will do is probably
  // not what the user had in mind.
  if (cNodes.size() & tgNodes.size() || tgNodes.size() & fbNodes.size())
  {
    QMessageBox::warning(QApplication::activeWindow(), "MI-Brain",
      "You must select either only the Tracts node, or one or more tract "
      "groups or one or more tracts. You can't mix them.");
    return;
  }

  for (auto node : boost::join(cNodes, boost::join(tgNodes, fbNodes)))
  {
    node->SetStringProperty("Save", SaveToWhat());
  }
}
