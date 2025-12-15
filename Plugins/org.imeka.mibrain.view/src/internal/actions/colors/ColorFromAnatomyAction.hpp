
#ifndef MIBRAIN_COLOR_FROM_ANAT_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_FROM_ANAT_ACTION_HPP_INCLUDED

#include <QObject>

#include "ColorAbstractAction.hpp"

class ColorFromAnatomyAction : public QObject, public ColorAbstractAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

protected:
  mitk::FilteredFiberBundle::Coloring GetColoring() const
  {
    return mitk::FilteredFiberBundle::Coloring::FromAnat;
  }

  bool NeedAnat() const { return true; }
};

#endif // MIBRAIN_COLOR_FROM_ANAT_ACTION_HPP_INCLUDED
