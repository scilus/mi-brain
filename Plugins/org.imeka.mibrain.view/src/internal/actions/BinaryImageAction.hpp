
#ifndef MIBRAIN_BINARY_IMAGE_ACTION_HPP_INCLUDED
#define MIBRAIN_BINARY_IMAGE_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class BinaryImageAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;

  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_BINARY_IMAGE_ACTION_HPP_INCLUDED
