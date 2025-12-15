#ifndef BRAIN_ANALYSIS_VIEW_HPP_INCLUDED
#define BRAIN_ANALYSIS_VIEW_HPP_INCLUDED

#include "ImekaFiber/FibersManager.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/MaximaData.hpp"

#include "CommonView.hpp"

#include "ui_BrainAnalysisViewControls.h"

class BrainAnalysisView : public CommonView
{
  Q_OBJECT

private:
  typedef Imeka::Fiber::SelectionObject SelectionObject;

signals:
  void RequestUpdateUI();

public:
  static const std::string VIEW_ID;
  BrainAnalysisView();
  virtual ~BrainAnalysisView();

  virtual void CreateQtPartControl(QWidget *parent) override;

protected slots:
  void AnatComboBoxNodeChanged(const mitk::DataNode*);
  void TractGroupVisibilityChanged(const bool);

  void UpdateUI();
  void DisplayStats();
  void ExportFibersStatsToCSV();

protected:
  virtual void DMSelectionChanged(
    const QVector<mitk::DataNode::Pointer>& nodes) override;
  virtual void NodeAdded(const mitk::DataNode* node) override;
  virtual void NodeRemoved(const mitk::DataNode* node) override;

  Ui::BrainAnalysisViewControls m_Controls;

private:
  void SetMaxFibersLength(const float, const bool);
  Imeka::Fiber::GroupNodes::ROIAction GetCreateROICallback() const;
  void RenderingManagerReinitialized();
  void SelectionObjectAdded(mitk::DataNode*, SelectionObject*);
  void PeaksImageAdded(mitk::DataNode*, mitk::Image*);

  Imeka::Callback m_Callback;
  Imeka::Fiber::GroupNodes m_Groups;
  Imeka::Fiber::FibersManager m_FibersManager;
  Imeka::Fiber::MaximaData& m_MaximaData;
  ConstNodes m_SelectedFiberBundles;
  unsigned int m_RenderingManagerObserverTag;
};

#endif // BRAIN_ANALYSIS_VIEW_HPP_INCLUDED
