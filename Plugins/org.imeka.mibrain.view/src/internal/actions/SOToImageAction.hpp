#ifndef MIBRAIN_SO_TO_IMAGE_ACTION_HPP_INCLUDED
#define MIBRAIN_SO_TO_IMAGE_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class SOToImageAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  SOToImageAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  SOToImageAction(const SOToImageAction &);
  SOToImageAction & operator=(const SOToImageAction &);

  mitk::DataStorage::Pointer m_DS;
};

#endif // MIBRAIN_SO_TO_IMAGE_ACTION_HPP_INCLUDED
