
#ifndef MIBRAIN_CREATE_SPHERE_SO_ACTION_HPP_INCLUDED
#define MIBRAIN_CREATE_SPHERE_SO_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class CreateSphereSOAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  CreateSphereSOAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  CreateSphereSOAction(const CreateSphereSOAction &);
  CreateSphereSOAction & operator=(const CreateSphereSOAction &);
};

#endif // MIBRAIN_CREATE_SPHERE_SO_ACTION_HPP_INCLUDED
