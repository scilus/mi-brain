
#ifndef MIBRAIN_COLOR_END_POINTS_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_END_POINTS_ACTION_HPP_INCLUDED

#include <QObject>

#include "ColorAbstractAction.hpp"

class ColorEndPointsAction : public QObject, public ColorAbstractAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

protected:
  mitk::FilteredFiberBundle::Coloring GetColoring() const
  {
    return mitk::FilteredFiberBundle::Coloring::EndPoint;
  }

  bool NeedAnat() const { return false; }
};

#endif // MIBRAIN_COLOR_END_POINTS_ACTION_HPP_INCLUDED
