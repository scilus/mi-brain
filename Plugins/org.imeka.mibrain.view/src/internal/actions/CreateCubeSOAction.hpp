
#ifndef MIBRAIN_CREATE_CUBE_SO_ACTION_HPP_INCLUDED
#define MIBRAIN_CREATE_CUBE_SO_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class CreateCubeSOAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  CreateCubeSOAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  CreateCubeSOAction(const CreateCubeSOAction &);
  CreateCubeSOAction & operator=(const CreateCubeSOAction &);
};

#endif // MIBRAIN_CREATE_CUBE_SO_ACTION_HPP_INCLUDED
