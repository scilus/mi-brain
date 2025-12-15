
#ifndef MIBRAIN_COLOR_UNIFORM_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_UNIFORM_ACTION_HPP_INCLUDED

#include <QColorDialog>
#include <QObject>

#include <mitkIContextMenuAction.h>
#include <mitkRenderingManager.h>

#include "ImekaCommon/Colors.hpp"
#include "ImekaFiber/Coloring/FibersColors.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"

class ColorUniformAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override
  {
    if (selectedNodes.size() == 1
      && Imeka::Fiber::IsTractsCategory(selectedNodes[0]))
    {
      const auto tractsCategoryNode = selectedNodes[0];
      const auto nodes =
        Imeka::DataManager(m_DS).DirectChildrenOf(tractsCategoryNode);
      if (nodes.size() == 0) { return; }

      const QColor color = QColorDialog::getColor(
        Imeka::Fiber::GetSingleInitialColor(nodes));
      if (!color.isValid()) { return; }

      Imeka::Color::Apply(tractsCategoryNode, color);
    }
    else if (selectedNodes.size() == 1
      && Imeka::Fiber::IsTractGroup(selectedNodes[0]))
    {
      const auto tgNode = selectedNodes[0];
      const auto datasetNode = m_DS->GetSources(tgNode)->front();
      const QColor color = QColorDialog::getColor(
        Imeka::Fiber::GetInitialColor(datasetNode, tgNode));
      if (!color.isValid()) { return; }

      Imeka::Color::Apply(tgNode, color);
    }
    else
    {
      const QColor color = QColorDialog::getColor(
        Imeka::Fiber::GetSingleInitialColor(selectedNodes));
      if (!color.isValid()) { return; }

      for (const auto node : selectedNodes)
      {
        Imeka::Color::Apply(node, color);
      }
    }

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }

  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_COLOR_UNIFORM_ACTION_HPP_INCLUDED
