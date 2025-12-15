
#ifndef MIBRAIN_COLOR_SHUFFLE_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_SHUFFLE_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>
#include <mitkRenderingManager.h>

#include "ImekaCommon/Colors.hpp"
#include "ImekaFiber/utils.hpp"
#include "ImekaFiber/GroupNodes.hpp"

class ColorShuffleAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override
  {
    if (selectedNodes.size() == 1)
    {
      const auto node = selectedNodes[0];
      if (Imeka::Fiber::IsTractsCategory(node) || Imeka::Fiber::IsROIsCategory(node))
      {
        node->SetIntProperty("ShuffleColor", 1);
      }
      else
      {
        Imeka::Color::ShuffleColors({ node });
      }
    }
    else
    {
      Nodes nodes;
      for (auto node : selectedNodes)
      {
        const bool isNotCategory =
             !Imeka::Fiber::IsTractsCategory(node)
          && !Imeka::Fiber::IsROIsCategory(node);
        const bool tractOrROI =
             Imeka::Fiber::GetTractPredicate()->CheckNode(node)
          || Imeka::Fiber::GetROIPredicate()->CheckNode(node);
        if (isNotCategory && tractOrROI) { nodes.push_back(node); }
      }
      Imeka::Color::ShuffleColors(nodes);
    }

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }

  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}
};

#endif // MIBRAIN_COLOR_SHUFFLE_ACTION_HPP_INCLUDED
