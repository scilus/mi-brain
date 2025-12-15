#ifndef COMMONVIEW_LOCATE_SO_ACTION_HPP_INCLUDED
#define COMMONVIEW_LOCATE_SO_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class LocateSelectionObjectAction
  : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  LocateSelectionObjectAction() {}
  ~LocateSelectionObjectAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  LocateSelectionObjectAction(const LocateSelectionObjectAction &);
  LocateSelectionObjectAction & operator=(const LocateSelectionObjectAction &);
};

#endif // COMMONVIEW_LOCATE_SO_ACTION_HPP_INCLUDED
