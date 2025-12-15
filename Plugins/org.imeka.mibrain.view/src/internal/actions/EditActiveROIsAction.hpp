
#ifndef MIBRAIN_EDIT_ACTIVE_ROIS_ACTION_HPP_INCLUDED
#define MIBRAIN_EDIT_ACTIVE_ROIS_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class EditActiveROIsAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  EditActiveROIsAction()
    : m_DS(nullptr)
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

  void OnSurfaceCalculationDone();

private:
  EditActiveROIsAction(const EditActiveROIsAction &);
  EditActiveROIsAction & operator=(const EditActiveROIsAction &);

  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_EDIT_ACTIVE_ROIS_ACTION_HPP_INCLUDED
