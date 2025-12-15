#include "BrainRTTView.hpp"

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Mapper/Mappers2DSettingsWidget.hpp"
#include "ImekaFiber/Saver.hpp"
#include "ImekaFiber/utils.hpp"
#include "ImekaWidgets/Qt/DataNodeAndPropertiesTableModel.hpp"
#include "ImekaWidgets/MITK/NodeTableViewUtils.hpp"

const std::string BrainRTTView::VIEW_ID = "org.mitk.views.brainrttview";

BrainRTTView::BrainRTTView()
  : m_MaximaData(Imeka::Fiber::MaximaData::Instance())
  , m_MaximaNode(nullptr)
{
  Imeka::Fiber::Saver::Instance().AddNodes(&m_RTTNodeData, true);
}

BrainRTTView::~BrainRTTView()
{
  while (m_RTTNodeData.size() > 0)
  {
    m_DM.RemoveNode(m_RTTNodeData.firstKey());
  }
  Imeka::Fiber::Saver::Instance().RemoveRTTNodes();
  Imeka::Fiber::Mappers2DSettingsWidget::Instance->RemoveNodes(&m_RTTNodeData);

  // Prevent a crash when:
  // 1) this plugin was open/visible and the user closes it.
  // 2) User activate NT, which would activate the SLOT below.
  QObject::disconnect(m_NTConnection);
}

void BrainRTTView::CreateQtPartControl(QWidget *parent)
{
  m_Controls.setupUi(parent);

  m_Controls.lblTensors->hide();
  m_Controls.cboTensorsImage->hide();

  /* The isAnat predicate is a little different here. An anat can normally be
     any kind of image but here we want any kind of image except those
     considered peaks image and binary images. */
  auto isPeaksImage = Imeka::Fiber::GetMaximaPredicate();
  auto isAnat = mitk::NodePredicateAnd::New();
  isAnat->AddPredicate(Imeka::Fiber::GetAnatPredicate());
  isAnat->AddPredicate(mitk::NodePredicateNot::New(isPeaksImage));
  isAnat->AddPredicate(Imeka::Predicate::Property("binary", false));

  m_Controls.cboTrackingMaskImage->SetDataStorage(GetDataStorage());
  m_Controls.cboTrackingMaskImage->SetPredicate(isAnat);
  connect(m_Controls.cboTrackingMaskImage,
    &QmitkDataStorageComboBox::OnSelectionChanged,
    this, &BrainRTTView::AnatComboBoxNodeChanged);

  m_Controls.cboTensorsImage->SetDataStorage(GetDataStorage());
  m_Controls.cboTensorsImage->SetPredicate(
    Imeka::Fiber::GetTensorsPredicate());
  connect(m_Controls.cboTensorsImage,
    &QmitkDataStorageComboBox::OnSelectionChanged,
    this, &BrainRTTView::TensorsComboBoxNodeChanged);

  m_Controls.cboMaximaImage->SetDataStorage(GetDataStorage());
  m_Controls.cboMaximaImage->SetPredicate(isPeaksImage);
  connect(m_Controls.cboMaximaImage,
    &QmitkDataStorageComboBox::OnSelectionChanged,
    this, &BrainRTTView::MaximaComboBoxNodeChanged);

  m_NTConnection = connect(
    &m_MaximaData, &Imeka::Fiber::MaximaData::Reloaded,
    [this](){ Update(false); });

  connect(m_Controls.exportFibersStatsToCSV, &QAbstractButton::clicked,
    this, &BrainRTTView::ExportFibersStatsToCSV);

  m_Controls.tvBinaryMasks->setEnabled(false);
  m_Controls.tvBinaryMasks->setModel(
    new Imeka::Widgets::DataNodeAndPropertiesTableModel(
    GetDataStorage(),
    Imeka::Fiber::GetROIPredicate(),
    m_ParentWidget,
    QStringList() << "Name" << "Color"));
  connect(m_Controls.tvBinaryMasks->selectionModel(),
    &QItemSelectionModel::selectionChanged,
    this, &BrainRTTView::ROINodesSelectionChanged);
  m_Controls.tvBinaryMasks->horizontalHeader()->setSectionResizeMode(
    QHeaderView::Stretch);

  // Simply connect the sliders to the spinboxe and they will handle the
  // complexity. They always have the right value and type, not the sliders.
  connect(
    m_Controls.sldMinFiberLength, &QAbstractSlider::valueChanged,
    m_Controls.spnMinFiberLength, &QSpinBox::setValue);
  connect(
    m_Controls.sldMaxFiberLength, &QAbstractSlider::valueChanged,
    m_Controls.spnMaxFiberLength, &QSpinBox::setValue);
  connect(
    m_Controls.sldCurvatureThreshold, &QAbstractSlider::valueChanged,
    m_Controls.spnCurvatureThreshold, &QSpinBox::setValue);
  connect(
    m_Controls.sldMinTrackingMask, &QAbstractSlider::valueChanged, [this](){
    m_Controls.spnMinTrackingMask->setValue(
      m_Controls.sldMinTrackingMask->value() / 100.0);
  });
  connect(
    m_Controls.sldStepLength, &QAbstractSlider::valueChanged, [this](){
    m_Controls.spnStepLength->setValue(
      m_Controls.sldStepLength->value() / 20.0);
  });
  connect(
    m_Controls.sldSeedsPerAxis, &QAbstractSlider::valueChanged,
    m_Controls.spnSeedsPerAxis, &QSpinBox::setValue);
  connect(
    m_Controls.sldPuncture, &QAbstractSlider::valueChanged, [this](){
    m_Controls.spnPuncture->setValue(
      m_Controls.sldPuncture->value() / 100.0);
  });

  // SpinBoxes
  auto valueChangedInt = QOverload<int>::of(&QSpinBox::valueChanged);
  auto valueChangedDouble =
    QOverload<double>::of(&QDoubleSpinBox::valueChanged);
  connect(m_Controls.spnMinFiberLength, valueChangedInt,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnMaxFiberLength, valueChangedInt,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnCurvatureThreshold, valueChangedInt,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnMinTrackingMask, valueChangedDouble,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnStepLength, valueChangedDouble,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnSeedsPerAxis, valueChangedInt,
    this, &BrainRTTView::SpinBoxesToParameters);
  connect(m_Controls.spnPuncture, valueChangedDouble,
    this, &BrainRTTView::SpinBoxesToParameters);
  SpinBoxesToParameters();

  AnatComboBoxNodeChanged(m_Controls.cboTrackingMaskImage->GetSelectedNode());
  TensorsComboBoxNodeChanged(m_Controls.cboTensorsImage->GetSelectedNode());
  MaximaComboBoxNodeChanged(m_Controls.cboMaximaImage->GetSelectedNode());
}

void BrainRTTView::SpinBoxesToParameters()
{
  auto& params = Imeka::Fiber::RTT::parameters;

  // NOTE to programmer: * is applied after the =
  m_Controls.sldMinFiberLength->setValue(
    params.minLength = m_Controls.spnMinFiberLength->value());
  m_Controls.sldMaxFiberLength->setValue(
    params.maxLength = m_Controls.spnMaxFiberLength->value());
  m_Controls.sldCurvatureThreshold->setValue(
    params.maxAngle = m_Controls.spnCurvatureThreshold->value());
  // 100.01 to account for numerical imprecision.
  m_Controls.sldMinTrackingMask->setValue(
    (params.minFA = m_Controls.spnMinTrackingMask->value()) * 100.01);
  m_Controls.sldStepLength->setValue(
    (params.step = m_Controls.spnStepLength->value()) * 20.0);
  m_Controls.sldSeedsPerAxis->setValue(
    params.nbSeeds = m_Controls.spnSeedsPerAxis->value());
  m_Controls.sldPuncture->setValue(
    (params.puncture = m_Controls.spnPuncture->value()) * 100.0);

  Update(false);
}

void BrainRTTView::UpdateUI()
{
  const mitk::Image* trackingMask = GetTrackingMask();
  if (!trackingMask || !m_MaximaNode)
  {
    m_Controls.wdgPeaksWarning->setVisible(false);
    m_Controls.tvBinaryMasks->setEnabled(false);
    m_Controls.tvBinaryMasks->clearSelection();
    m_SelectedNodes.clear();

    for (const auto RTTNode : m_RTTNodeData.keys())
    {
      m_DM.RemoveNode(RTTNode);
    }
    m_RTTNodeData.clear();
    m_RTT.clear();

    return;
  }

  const auto maximaGeo =
    dynamic_cast<mitk::Image*>(m_MaximaNode->GetData())->GetGeometry();
  const auto tmSpacing = trackingMask->GetGeometry()->GetSpacing();
  m_Controls.wdgPeaksWarning->setVisible(
    tmSpacing != maximaGeo->GetSpacing());
  m_Controls.tvBinaryMasks->setEnabled(true);
}

void BrainRTTView::FillRTTNodes(Imeka::Fiber::RTT* RTT)
{
  if (!m_MaximaNode) { return; }

  const mitk::Image* trackingMask = GetTrackingMask();
  const mitk::Image* tensorsImage = GetTensorsImage();
  auto& maxima = m_MaximaData.Get(m_MaximaNode);
  if (RTT)
  {
    RTT->SetTrackingMaskGeometry(trackingMask->GetGeometry());
    RTT->SetTensorsData(tensorsImage);
    RTT->SetMaxima(&maxima);
    return;
  }

  // Qt doesn't offer a way to iterate mutably on values
  for (auto ROINode : m_RTT.keys())
  {
    auto& RTT = m_RTT[ROINode].RTT;
    RTT.SetTrackingMaskGeometry(trackingMask->GetGeometry());
    RTT.SetTensorsData(tensorsImage);
    RTT.SetMaxima(&maxima);
  }
}

void BrainRTTView::NodeRemoved(const mitk::DataNode* node)
{
  auto it = m_RTT.find(node);
  if (it != m_RTT.end()) // Node is a ROI
  {
    m_RTTNodeData.erase(m_RTTNodeData.find(it->RTTNode));

    m_DM.RemoveNode(it->RTTNode);
    m_RTT.erase(it);

    // May be already removed if ROINodesSelectionChanged is called before.
    auto itNode = std::find(
      std::begin(m_SelectedNodes), std::end(m_SelectedNodes), node);
    if (itNode != std::end(m_SelectedNodes))
    {
      m_SelectedNodes.erase(itNode);
    }
  }
  else
  {
    auto it = m_RTTNodeData.find(node);
    if (it != m_RTTNodeData.end()) // Node is a hidden RTT node
    {
      m_RTTNodeData.erase(it);
    }
  }
}

void BrainRTTView::AnatComboBoxNodeChanged(const mitk::DataNode* node)
{
  if (node) { FillRTTNodes(); }
  Imeka::Fiber::RTT::trackingMask.SetImage(GetTrackingMask());
  UpdateUI();
}

void BrainRTTView::TensorsComboBoxNodeChanged(const mitk::DataNode*)
{
  FillRTTNodes();
  UpdateUI();
}

void BrainRTTView::MaximaComboBoxNodeChanged(const mitk::DataNode* node)
{
  if (node)
  {
    m_MaximaNode = m_Controls.cboMaximaImage->GetSelectedNode();
    m_MaximaData.Add(m_MaximaNode);
    FillRTTNodes();
  }
  else
  {
    m_MaximaData.Remove(m_MaximaNode);
    m_MaximaNode = nullptr;
  }
  UpdateUI();
  Update(false);
}

void BrainRTTView::ROINodesSelectionChanged(
  const QItemSelection& selected,
  const QItemSelection& deselected)
{
  // Should be in the constructor but this plugin can be created before the
  // BrainAnalysisView plugin, so Instance would be null. Yes, this can be
  // called many times without problem.
  Imeka::Fiber::Mappers2DSettingsWidget::Instance->AddNodes(&m_RTTNodeData);

  // Clean the nodes that were deselected
  for (auto ROINode :
    Imeka::Widgets::IndicesToNodes(m_Controls.tvBinaryMasks, deselected))
  {
    if (dynamic_cast<SelectionObject*>(ROINode->GetData()))
    {
      RemoveSOActions(ROINode);
    }
    m_RTT[ROINode].RTT.Clear();
  }

  // Do not update anything if we removed required nodes
  if (!GetTrackingMask() || !m_MaximaNode) { return; }

  Imeka::Widgets::UpdateSelected(
    m_Controls.tvBinaryMasks, selected, deselected, m_SelectedNodes);
  Update(true);
}

void BrainRTTView::Update(const bool create)
{
  for (auto ROINode : m_SelectedNodes)
  {
    const bool exist = m_RTT.contains(ROINode);
    if (!exist)
    {
      SetupNewRTTNode(ROINode);
    }

    if (const auto SO = dynamic_cast<SelectionObject*>(ROINode->GetData()))
    {
      if (create)
      {
        AddSOActions(ROINode);
      }
      SelectionObjectReleased(SO);
    }
    else if (const auto mask = dynamic_cast<mitk::Image*>(ROINode->GetData()))
    {
      auto &rttData = m_RTT[ROINode];
      rttData.RTT.SeedFromMask(mask);
      ApplyChangesToPolyData(rttData);
      rttData.RTTFibers->CalculateStatsUsingVisibility();
    }
  }

  DisplayStats();
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void BrainRTTView::SetupNewRTTNode(mitk::DataNode* ROINode)
{
  auto RTTNode = mitk::DataNode::New();
  RTTNode->SetName(ROINode->GetName());

  auto fiber = mitk::FilteredFiberBundle::New(true);

  m_RTT[ROINode] = RTTData{ RTTNode, fiber, Imeka::Fiber::RTT() };
  auto& RTT = m_RTT[ROINode].RTT;
  RTT.SetRTTFibers(fiber);
  FillRTTNodes(&RTT);

  RTTNode->SetData(fiber);
  RTTNode->SetBoolProperty("helper object", true);
  RTTNode->SetBoolProperty("includeInBoundingBox", false);
  m_DM.AddNode(RTTNode);

  Imeka::Fiber::SetupNodeDataAndMappers(
    RTTNode, fiber, m_RTTNodeData[RTTNode]);
}

void BrainRTTView::AddSOActions(const mitk::DataNode* ROINode)
{
  auto interactor = dynamic_cast<Interactor*>(
    ROINode->GetDataInteractor().GetPointer());
  connect(interactor, &Interactor::HasBeenChanged,
    this, &BrainRTTView::SelectionObjectMoved);
  connect(interactor, &Interactor::Unselected,
    this, &BrainRTTView::SelectionObjectReleased);
}

void BrainRTTView::RemoveSOActions(const mitk::DataNode* ROINode)
{
  auto interactor = dynamic_cast<Interactor*>(
    ROINode->GetDataInteractor().GetPointer());
  disconnect(interactor, &Interactor::HasBeenChanged,
    this, &BrainRTTView::SelectionObjectMoved);
  disconnect(interactor, &Interactor::Unselected,
    this, &BrainRTTView::SelectionObjectReleased);
}

// Called each time a SelectionObject moves.
void BrainRTTView::SelectionObjectMoved(SelectionObject* SO)
{
  const auto ROINode = m_DM.GetNodeContainingThis(SO);
  auto& rttData = m_RTT[ROINode];
  rttData.RTT.SeedFromSelectionObject(SO);
  ApplyChangesToPolyData(rttData);
}

void BrainRTTView::SelectionObjectReleased(SelectionObject* SO)
{
  SelectionObjectMoved(SO);

  const auto ROINode = m_DM.GetNodeContainingThis(SO);
  auto fiber = m_RTT[ROINode].RTTFibers;
  fiber->CalculateStatsUsingVisibility();
  DisplayStats();
}

void BrainRTTView::ApplyChangesToPolyData(RTTData& rttData)
{
  auto polyData = rttData.RTTFibers->GetFiberPolyData();
  rttData.RTTFibers->GetFiberColors()->Modified();
  rttData.RTTFibers->RequestUpdate3D();
  polyData->BuildCells();
  polyData->GetPoints()->Modified();
  polyData->GetLines()->Modified();
}

void BrainRTTView::DisplayStats()
{
  QString stats("");
  for (auto ROINode : m_SelectedNodes)
  {
    // Is there's a missing node, simply quit. It will be called again soon.
    if (!m_RTT.contains(ROINode))
    {
      m_Controls.txtFibersStats->setPlainText("");
      return;
    }

    auto fiber = m_RTT[ROINode].RTTFibers;

    QString stddev = "\n";
    if (fiber->GetNumFibers() > 1)
    {
      stddev = QString::number(fiber->GetLengthStDev(), 'f', 1) + " mm\n";
    }
    stats += QString(ROINode->GetName().c_str()) + "\n" +
      "Num. of fibers: " +
      QString::number(fiber->GetNumFibers()) + "\n" +
      "Min. length:    " +
      QString::number(fiber->GetMinFiberLength(), 'f', 1) + " mm\n" +
      "Max. length:    " +
      QString::number(fiber->GetMaxFiberLength(), 'f', 1) + " mm\n" +
      "Mean length:    " +
      QString::number(fiber->GetMeanFiberLength(), 'f', 1) + " mm\n" +
      "Median length:  " +
      QString::number(fiber->GetMedianFiberLength(), 'f', 1) + " mm\n" +
      "Std. deviation: " + stddev +
      "-----------------------------\n";
  }
  m_Controls.txtFibersStats->setPlainText(stats);
}

void BrainRTTView::ExportFibersStatsToCSV()
{
  if (m_RTT.empty()) { return; }

  ConstNodes nodes;
  for (auto ROINode : m_SelectedNodes)
  {
    nodes.push_back(m_RTT[ROINode].RTTNode);
  }
  Imeka::Fiber::ExportFibersStatsToCSV(nodes, true);
}
