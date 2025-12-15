
#ifndef MIBRAIN_SAVE_FIBERS_TO_FILE_ACTION_HPP_INCLUDED
#define MIBRAIN_SAVE_FIBERS_TO_FILE_ACTION_HPP_INCLUDED

#include <QObject>

#include "SaveVisibleFibersAbstractAction.hpp"

class SaveFibersToFileAction : public QObject, public SaveVisibleFibersAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  virtual const char* SaveToWhat()
  {
    return "file";
  }
};

#endif // MIBRAIN_SAVE_FIBERS_TO_FILE_ACTION_HPP_INCLUDED
