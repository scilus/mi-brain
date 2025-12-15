
#ifndef MIBRAIN_SAVE_VISIBLE_FIBERS_ACTION_HPP_INCLUDED
#define MIBRAIN_SAVE_VISIBLE_FIBERS_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class SaveVisibleFibersAction : public mitk::IContextMenuAction
{
public:
  SaveVisibleFibersAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override { }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

  virtual const char* SaveToWhat() = 0;

private:
  SaveVisibleFibersAction(const SaveVisibleFibersAction &);
  SaveVisibleFibersAction & operator=(const SaveVisibleFibersAction &);
};

#endif // MIBRAIN_SAVE_VISIBLE_FIBERS_ACTION_HPP_INCLUDED
