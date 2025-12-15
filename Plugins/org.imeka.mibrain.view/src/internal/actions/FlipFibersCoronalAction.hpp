
#ifndef MIBRAIN_FLIP_FIBERS_CORONAL_ACTION_HPP_INCLUDED
#define MIBRAIN_FLIP_FIBERS_CORONAL_ACTION_HPP_INCLUDED

#include <QObject>

#include "FlipFibersAbstractAction.hpp"

class FlipFibersCoronalAction
  : public QObject, public FlipFibersAbstractAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

protected:
  int GetAxis() const { return 1; }
};

#endif // MIBRAIN_FLIP_FIBERS_CORONAL_ACTION_HPP_INCLUDED
