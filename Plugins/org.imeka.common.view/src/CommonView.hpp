
#ifndef IMEKA_VIEW_HPP_INCLUDED
#define IMEKA_VIEW_HPP_INCLUDED

#include <QmitkAbstractView.h>

#include "ImekaCommon/DataManager.hpp"

#include "org_imeka_common_view_Export.h"

class QAction;
class QToolBar;
class QToolButton;
class QmitkStdMultiWidget;

namespace mitk { class PlaneGeometry; }

class COMMONVIEW_EXPORT CommonView : public QmitkAbstractView
{
  Q_OBJECT

public:
  CommonView();
  virtual ~CommonView() {}

protected:
  virtual void SetFocus() override {}

  virtual void DMSelectionChanged(
    const QVector<mitk::DataNode::Pointer>& nodes) = 0;

  void CallNodeAddedOnAllNodes();
  QmitkStdMultiWidget* GetStdMultiWidget();
  mitk::PlaneGeometry* GetPlaneGeometry(const char*) const;

  unsigned int GetCurrentTimeStep() const;
  void GlobalReinit();
  void Reinit(const mitk::DataNode*);
  bool IsReinited(const mitk::DataNode*) const;

  QWidget* m_ParentWidget;
  Imeka::DataManager m_DM;

private:
  virtual void OnSelectionChanged(
    berry::IWorkbenchPart::Pointer source,
    const QList<mitk::DataNode::Pointer>& nodes) override;
  virtual void NodeAdded(const mitk::DataNode*) override {}
};

#endif // IMEKA_VIEW_HPP_INCLUDED
