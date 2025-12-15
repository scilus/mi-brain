
#ifndef IMEKA_VIEW_PLACER_ACTION_HPP_INCLUDED
#define IMEKA_VIEW_PLACER_ACTION_HPP_INCLUDED

#include <QAction>

#include "org_imeka_common_app_Export.h"

class QmitkFileOpenActionPrivate;

class COMMONAPP_EXPORT ViewPlacerAction : public QAction
{
  Q_OBJECT

public:
  enum ViewPosition {
    LEFT, RIGHT, SUPERIOR, INFERIOR, ANTERIOR, POSTERIOR
  };

  ViewPlacerAction(const ViewPosition, QWidget* = nullptr);

protected slots:
  virtual void Run();

private:
  const ViewPosition m_ViewPosition;
};

#endif // IMEKA_VIEW_PLACER_ACTION_HPP_INCLUDED
