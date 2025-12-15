
#ifndef MIBRAIN_TOGGLE_PEAKS_STATUS_ACTION_HPP_INCLUDED
#define MIBRAIN_TOGGLE_PEAKS_STATUS_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class TogglePeaksStatusAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  TogglePeaksStatusAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  TogglePeaksStatusAction(const TogglePeaksStatusAction &);
  TogglePeaksStatusAction & operator=(const TogglePeaksStatusAction &);

  mitk::DataStorage::Pointer m_DS;
};

#endif // MIBRAIN_TOGGLE_PEAKS_STATUS_ACTION_HPP_INCLUDED
