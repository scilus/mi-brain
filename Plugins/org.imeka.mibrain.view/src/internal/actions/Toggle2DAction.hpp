
#ifndef MIBRAIN_TOGGLE_2D_ACTION_HPP_INCLUDED
#define MIBRAIN_TOGGLE_2D_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class Toggle2DAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  Toggle2DAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override { }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  Toggle2DAction(const Toggle2DAction &);
  Toggle2DAction & operator=(const Toggle2DAction &);
};

#endif // MIBRAIN_TOGGLE_2D_ACTION_HPP_INCLUDED
