
#ifndef MIBRAIN_CREATE_TRACT_GROUP_ACTION_HPP_INCLUDED
#define MIBRAIN_CREATE_TRACT_GROUP_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class CreateTractGroupAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  CreateTractGroupAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  CreateTractGroupAction(const CreateTractGroupAction &);
  CreateTractGroupAction & operator=(const CreateTractGroupAction &);
};

#endif // MIBRAIN_CREATE_TRACT_GROUP_ACTION_HPP_INCLUDED
