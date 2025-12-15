#ifndef COMMONVIEW_LOCATE_SURFACE_ACTION_HPP_INCLUDED
#define COMMONVIEW_LOCATE_SURFACE_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class LocateSurfaceAction
  : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  LocateSurfaceAction() {}
  ~LocateSurfaceAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  LocateSurfaceAction(const LocateSurfaceAction &);
  LocateSurfaceAction & operator=(const LocateSurfaceAction &);
};

#endif // COMMONVIEW_LOCATE_SURFACE_ACTION_HPP_INCLUDED
