#ifndef MIBRAIN_SHUFFLE_TRACTS_ACTION_HPP_INCLUDED
#define MIBRAIN_SHUFFLE_TRACTS_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class ShuffleTractsAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  ShuffleTractsAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override {Q_UNUSED(ds);}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  ShuffleTractsAction(const ShuffleTractsAction &);
  ShuffleTractsAction & operator=(const ShuffleTractsAction &);
};

#endif // MIBRAIN_SHUFFLE_TRACTS_ACTION_HPP_INCLUDED
