
#ifndef IMEKA_TOGGLE_RGB_ACTION_HPP_INCLUDED
#define IMEKA_TOGGLE_RGB_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class ToggleRGBAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  ToggleRGBAction() {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage*) override {}
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  ToggleRGBAction(const ToggleRGBAction &);
  ToggleRGBAction & operator=(const ToggleRGBAction &);
};

#endif // IMEKA_TOGGLE_RGB_ACTION_HPP_INCLUDED
