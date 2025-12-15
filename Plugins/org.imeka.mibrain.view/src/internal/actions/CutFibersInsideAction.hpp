
#ifndef MIBRAIN_CUT_FIBERS_INSIDE_ACTION_HPP_INCLUDED
#define MIBRAIN_CUT_FIBERS_INSIDE_ACTION_HPP_INCLUDED

#include <QObject>

#include "CutFibersAbstractAction.hpp"

class CutFibersInsideAction : public QObject, public CutFibersAbstractAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

protected:
  virtual std::string GetCutName() const { return "Inside"; }
};

#endif // MIBRAIN_CUT_FIBERS_INSIDE_ACTION_HPP_INCLUDED
