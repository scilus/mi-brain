
#ifndef IMEKA_RELOAD_FROM_DISK_ACTION_HPP_INCLUDED
#define IMEKA_RELOAD_FROM_DISK_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class ReloadFromDiskAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  ReloadFromDiskAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  ReloadFromDiskAction(const ReloadFromDiskAction &);
  ReloadFromDiskAction & operator=(const ReloadFromDiskAction &);

  mitk::DataStorage::Pointer m_DS;
};

#endif // IMEKA_RELOAD_FROM_DISK_ACTION_HPP_INCLUDED
