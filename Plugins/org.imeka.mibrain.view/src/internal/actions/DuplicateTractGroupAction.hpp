
#ifndef MIBRAIN_DUPLICATE_TRACT_GROUP_ACTION_HPP_INCLUDED
#define MIBRAIN_DUPLICATE_TRACT_GROUP_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class DuplicateTractGroupAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  DuplicateTractGroupAction()
    : m_DS(nullptr) 
  {}

  void Run(const QList<mitk::DataNode::Pointer>& selectedNodes);

  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_DUPLICATE_TRACT_GROUP_ACTION_HPP_INCLUDED
