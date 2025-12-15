
#ifndef MIBRAIN_FLIP_FIBERS_AXIAL_ACTION_HPP_INCLUDED
#define MIBRAIN_FLIP_FIBERS_AXIAL_ACTION_HPP_INCLUDED

#include <QObject>

#include "FlipFibersAbstractAction.hpp"

class FlipFibersAxialAction
  : public QObject, public FlipFibersAbstractAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

protected:
  int GetAxis() const { return 2; }
};

#endif // MIBRAIN_FLIP_FIBERS_AXIAL_ACTION_HPP_INCLUDED
