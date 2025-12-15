
#ifndef MIBRAIN_SAVE_FIBERS_TO_DM_ACTION_HPP_INCLUDED
#define MIBRAIN_SAVE_FIBERS_TO_DM_ACTION_HPP_INCLUDED

#include <QObject>

#include "SaveVisibleFibersAbstractAction.hpp"

class SaveFibersToDMAction : public QObject, public SaveVisibleFibersAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  virtual const char* SaveToWhat()
  {
    return "dm";
  }
};

#endif // MIBRAIN_SAVE_FIBERS_TO_DM_ACTION_HPP_INCLUDED
