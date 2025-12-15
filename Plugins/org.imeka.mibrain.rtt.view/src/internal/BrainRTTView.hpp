#ifndef BRAIN_ANALYSIS_RTT_VIEW_HPP_INCLUDED
#define BRAIN_ANALYSIS_RTT_VIEW_HPP_INCLUDED

#include "ImekaBoundingObject/InteractorEmitter.hpp"
#include "ImekaFiber/FiberNodeData.hpp"
#include "ImekaFiber/MaximaData.hpp"
#include "ImekaFiber/RTT.hpp"
#include "ImekaFiber/types.hpp"
#include "CommonView.hpp"

#include "ui_BrainRTTViewControls.h"

class BrainRTTView : public CommonView
{
  Q_OBJECT

  typedef Imeka::Fiber::SelectionObject SelectionObject;
  typedef Imeka::BoundingObject::InteractorEmitter Interactor;

  struct RTTData
  {
    mitk::DataNode* RTTNode;
    mitk::FilteredFiberBundle* RTTFibers;
    Imeka::Fiber::RTT RTT;
  };

public:
  static const std::string VIEW_ID;
  BrainRTTView();
  virtual ~BrainRTTView();

  virtual void CreateQtPartControl(QWidget *parent) override;

protected slots:
  void AnatComboBoxNodeChanged(const mitk::DataNode*);
  void TensorsComboBoxNodeChanged(const mitk::DataNode*);
  void MaximaComboBoxNodeChanged(const mitk::DataNode*);
  void SpinBoxesToParameters();

  void ROINodesSelectionChanged(const QItemSelection&, const QItemSelection&);
  void SelectionObjectMoved(SelectionObject*);
  void SelectionObjectReleased(SelectionObject*);
  void ExportFibersStatsToCSV();

protected:
  virtual void DMSelectionChanged(
    const QVector<mitk::DataNode::Pointer>&) override {}
  virtual void NodeAdded(const mitk::DataNode*) override {}
  virtual void NodeRemoved(const mitk::DataNode* node) override;

  Ui::BrainRTTViewControls m_Controls;

private:
  mitk::Image* GetTrackingMask()
  {
    mitk::Image* trackingMask = nullptr;
    if (m_Controls.cboTrackingMaskImage->GetSelectedNode())
    {
      trackingMask = dynamic_cast<mitk::Image*>(
        m_Controls.cboTrackingMaskImage->GetSelectedNode()->GetData());
    }
    return trackingMask;
  }
  mitk::Image* GetTensorsImage()
  {
    mitk::Image* tensorsImage = nullptr;
    if (m_Controls.cboTensorsImage->GetSelectedNode())
    {
      tensorsImage = dynamic_cast<mitk::Image*>(
        m_Controls.cboTensorsImage->GetSelectedNode()->GetData());
    }
    return tensorsImage;
  }

  void ApplyChangesToPolyData(RTTData&);
  void UpdateUI();
  void FillRTTNodes(Imeka::Fiber::RTT* RTT = nullptr);
  void Update(const bool);
  void SetupNewRTTNode(mitk::DataNode*);
  void AddSOActions(const mitk::DataNode*);
  void RemoveSOActions(const mitk::DataNode*);
  void DisplayStats();

  QMetaObject::Connection m_NTConnection;
  Imeka::Fiber::MaximaData& m_MaximaData;
  Imeka::Fiber::NodeDataMap m_RTTNodeData;
  QMap<const mitk::DataNode*, RTTData> m_RTT;
  Nodes m_SelectedNodes;
  const mitk::DataNode* m_MaximaNode;
};

#endif // BRAIN_ANALYSIS_RTT_VIEW_HPP_INCLUDED
