
#ifndef ACTIVE_ROIS_DIALOG_BOX_HPP_INCLUDED
#define ACTIVE_ROIS_DIALOG_BOX_HPP_INCLUDED

#include <QDialog>
#include <QLabel>

#include "ImekaCommon/types.hpp"
#include "ImekaFiber/types.hpp"

#include "ImekaFiberExports.h"

class QCheckBox;
class QComboBox;
class QGridLayout;
class QScrollArea;

namespace Imeka
{

namespace Fiber
{

// Utility class
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(const QString &text)
    : QLabel(text, nullptr) {}
  ~ClickableLabel() {}

signals:
  void clicked();

protected:
  void mouseReleaseEvent(QMouseEvent*) {
    emit clicked();
  }
};

// Exported only because some tests access NodesToSelect directly and I
// had link error if I didn't export it.
class ImekaFiber_EXPORT ActiveROIsDialogBox : public QDialog
{
  Q_OBJECT

public:
  // Testing purpose only. The dialog won't appear and these nodes will be
  // selected automatically.
  typedef std::map<mitk::DataNode*, TractGroup::Line> AutoSetupLines;
  static const AutoSetupLines* LinesToAutoSetup;
  static void AutoSetup(
    TractGroup& tractGroup,
    const Nodes& allROINodes,
    const bool hasAnat);

  ActiveROIsDialogBox(QWidget*);

  bool GenerateDialogBox(
    TractGroup& tractGroup,
    const mitk::DataNode* newTractGroup,
    const Nodes& allROINodes,
    const bool hasAnat);

  const mitk::DataNode* GetTractGroup();

private:
  struct UILine
  {
    QCheckBox* enabled;
    ClickableLabel* name;
    QLabel* color;
    QComboBox* segmentSelection;
    QCheckBox* _not;
  };
  UILine GetUILine(
    const Nodes&, const TractGroup::Line&, const mitk::DataNode*) const;

  const mitk::DataNode* m_TractGroup;
  QLabel *m_Title1, *m_Title2, *m_Title3, *m_Title4;
  QScrollArea* m_ScrollArea;
  QWidget* m_OkButton;
};

Nodes ImekaFiber_EXPORT GetOrderedIndexes(const Nodes& ROIs, const bool);
bool ImekaFiber_EXPORT IsSurfaceAndWeCare(const mitk::DataNode*, const bool);

} // namespace Fiber

} // namespace Imeka

#endif // ACTIVE_ROIS_DIALOG_BOX_HPP_INCLUDED
