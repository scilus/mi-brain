#ifndef MIBRAIN_COLOR_SHUFFLE_MASKS_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_SHUFFLE_MASKS_ACTION_HPP_INCLUDED

#include <QApplication>
#include <QMessageBox>
#include <QObject>

#include <mitkIContextMenuAction.h>
#include <mitkRenderingManager.h>

#include "ImekaFiber/utils.hpp"

class ColorShuffleMasksAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override
  {
    if (selectedNodes.size() > 1)
    {
      QMessageBox::warning(
        QApplication::activeWindow(),
        "MI-Brain", "You must select only the ROIs node.");
      return;
    }
    
    selectedNodes[0]->SetIntProperty("ShuffleColor", 2);
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }

  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}
};

#endif // MIBRAIN_COLOR_SHUFFLE_MASKS_ACTION_HPP_INCLUDED
