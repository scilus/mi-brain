#include "BrainAnalysisView.h"


#include "ImekaBoundingObject/InteractorEmitter.hpp"
#include "ImekaCommon/RGBMapper.hpp"
#include "ImekaFiber/Mapper/PeakImageMapper2D.hpp"
#include "ImekaFiber/Saver.hpp"
#include "ImekaFiber/utils.hpp"
#include "FiberBundle/DataStorageUtils.hpp"

const std::string BrainAnalysisView::VIEW_ID =
  "org.imeka.views.brainanalysisview";

BrainAnalysisView::BrainAnalysisView()
  : m_Callback()
  , m_Groups(m_Callback, m_DM)
  , m_FibersManager(m_DM, m_Callback, m_Groups, GetCreateROICallback())
  , m_MaximaData(Imeka::Fiber::MaximaData::Instance())
  , m_RenderingManagerObserverTag(0)
{}

BrainAnalysisView::~BrainAnalysisView()
{
  m_Controls.mappers2DSettings->RemoveNodes(&m_FibersManager.GetNodesMap());
  mitk::RenderingManager::GetInstance()
    ->RemoveObserver(m_RenderingManagerObserverTag);
}

Imeka::Callback::CallbackFunction
BrainAnalysisView::GetCreateROICallback() const
{
  return [this](mitk::DataNode* node)
  {
    std::string type = "";
    if (node->GetStringProperty("Create", type) && !type.empty())
    {
      m_Groups.ROIs->SetStringProperty("Create", "");
      auto automaticResizerAndRenamer = [this](mitk::DataNode* newSONode)
      {
        // Create the name of the SO with the appropriate cube/sphere number.
        // For example, the 3rd cube will be named : Selection Cube 03. The
        // extra "0" is added to maintain the alphabetical order.
        const std::string type = newSONode->GetData()->GetNameOfClass();
        QString pattern = QString("Selection ")
          + ((type == "Cuboid") ? "Cube " : "Sphere ") + " %1";
        newSONode->SetName(m_DM.FirstFreeName(pattern, 1, 2));
      };
      m_Controls.regionOfInterestWidget->CreateBO(
        type, automaticResizerAndRenamer);
    }
  };
}

void BrainAnalysisView::CreateQtPartControl(QWidget *parent)
{
  m_ParentWidget = parent;
  m_Controls.setupUi(m_ParentWidget);

  // Listen for Reinit and Global Reinit
  auto command = itk::SimpleMemberCommand<BrainAnalysisView>::New();
  command->SetCallbackFunction(
    this, &BrainAnalysisView::RenderingManagerReinitialized);
  m_RenderingManagerObserverTag =
    mitk::RenderingManager::GetInstance()->AddObserver(
      mitk::RenderingManagerViewsInitializedEvent(), command);

  m_Controls.mappers2DSettings->AddNodes(&m_FibersManager.GetNodesMap());

  m_Controls.regionOfInterestWidget->SetParentNode(m_Groups.ROIs);
  m_Controls.regionOfInterestWidget->HideActionButtons();
  m_Controls.regionOfInterestWidget->SetDataStorage(GetDataStorage());
  m_Controls.regionOfInterestWidget->SetOneBOMax(false);
  m_Controls.regionOfInterestWidget->AddTypeToRemove("Cone");
  m_Controls.regionOfInterestWidget->AddTypeToRemove("Cylinder");
  connect(m_Controls.regionOfInterestWidget,
    &Imeka::Widgets::RegionOfInterestWidget::PreROICreated,
    [this](SelectionObject* selectionObject)
  {
    auto geo = selectionObject->GetGeometry();
    const mitk::Point3D origin = GetRenderWindowPart()->GetSelectedPosition();

    // Can happen if there's nothing loaded and the user creates a SO,
    // GetSelectedPosition() will return scraps.
    if (fabs(origin[0]) >= 9999.0
     || fabs(origin[1]) >= 9999.0
     || fabs(origin[2]) >= 9999.0)
    {
      geo->SetOrigin(0.0);
      return;
    }

    // Change the size and position of all new selection boxes
    const auto anatNode = m_Controls.cboAnatImage->GetSelectedNode();
    // TODO Seach for the real spacing in the TRK
    const auto anatSpacing = (anatNode)
      ? anatNode->GetData()->GetGeometry()->GetSpacing()
      : mitk::Vector3D(1.0);
    geo->SetSpacing(2.5 * anatSpacing);
    geo->SetOrigin(origin);
  });
  connect(m_Controls.regionOfInterestWidget,
    &Imeka::Widgets::RegionOfInterestWidget::ROIHasBeenMoved,
    [this](SelectionObject* so)
  {
    m_FibersManager.SelectionObjectMoved(so, true);
  });
  connect(m_Controls.regionOfInterestWidget,
    &Imeka::Widgets::RegionOfInterestWidget::ROIHasBeenResized,
    [this](SelectionObject* so)
  {
    m_FibersManager.SelectionObjectMoved(so, true);
  });

  connect(m_Controls.chkShowAll, &QCheckBox::checkStateChanged, [this](){
    TractGroupVisibilityChanged(false);
  });
  connect(this, &BrainAnalysisView::RequestUpdateUI,
    this, &BrainAnalysisView::UpdateUI);
  connect(&m_FibersManager, &Imeka::Fiber::FibersManager::DisplayStats,
    this, &BrainAnalysisView::DisplayStats);
  connect(m_Controls.exportFibersStatsToCSV, &QAbstractButton::clicked,
    this, &BrainAnalysisView::ExportFibersStatsToCSV);

  auto valueChangedInt = QOverload<int>::of(&QSpinBox::valueChanged);
  connect(m_Controls.cboSkipPercentage,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](){ TractGroupVisibilityChanged(true); });
  connect(m_Controls.spnSkipUpTo, valueChangedInt, [this](){
    TractGroupVisibilityChanged(true);
  });

  m_Controls.cboAnatImage->SetDataStorage(GetDataStorage());
  m_Controls.cboAnatImage->SetPredicate(Imeka::Fiber::GetAnatPredicate());
  connect(m_Controls.cboAnatImage,
    &QmitkDataStorageComboBox::OnSelectionChanged,
    this, &BrainAnalysisView::AnatComboBoxNodeChanged);
  AnatComboBoxNodeChanged(m_Controls.cboAnatImage->GetSelectedNode());

  // Simply connect the sliders to the spinboxe and they will handle the
  // complexity. They always have the right value and type, not the sliders.
  connect(
    m_Controls.sldMinFiberLength, &QAbstractSlider::valueChanged,
    m_Controls.spnMinFiberLength, &QSpinBox::setValue);
  connect(
    m_Controls.sldMaxFiberLength, &QAbstractSlider::valueChanged,
    m_Controls.spnMaxFiberLength, &QSpinBox::setValue);

  // SpinBoxes
  connect(m_Controls.spnMinFiberLength, valueChangedInt, [this]() {
    const auto newValue = m_Controls.spnMinFiberLength->value();
    m_Controls.sldMinFiberLength->setValue(newValue);
    m_FibersManager.SetFilteringLengths(newValue, -1.0);
    if (newValue > m_Controls.spnMaxFiberLength->value())
    {
      m_Controls.spnMaxFiberLength->setValue(newValue);
    }
  });
  connect(m_Controls.spnMaxFiberLength, valueChangedInt, [this]() {
    const auto newValue = m_Controls.spnMaxFiberLength->value();
    m_Controls.sldMaxFiberLength->setValue(newValue);
    m_FibersManager.SetFilteringLengths(-1.0, newValue);
    if (newValue < m_Controls.spnMinFiberLength->value())
    {
      m_Controls.spnMinFiberLength->setValue(newValue);
    }
  });
  m_FibersManager.SetFilteringLengths(
    m_Controls.spnMinFiberLength->value(),
    m_Controls.spnMaxFiberLength->value());
  TractGroupVisibilityChanged(false);

  DataStorageSingleton::dataStorage = GetDataStorage();

  this->CallNodeAddedOnAllNodes();
  emit RequestUpdateUI();
}

void BrainAnalysisView::RenderingManagerReinitialized()
{
  const auto anatNode = m_Controls.cboAnatImage->GetSelectedNode();
  if (!anatNode || IsReinited(anatNode)) { return;  }

  // We jsut had a Global Reinit so we must hide all peaks node
  for (auto node : m_DM.GetAll(Imeka::Fiber::GetMaximaPredicate()))
  {
    node->SetVisibility(false);
  }
}

void BrainAnalysisView::TractGroupVisibilityChanged(
  const bool modified) // all params except showAll
{
  bool showAll = m_Controls.chkShowAll->isChecked();
  if (modified)
  {
    m_Controls.chkShowAll->setChecked(false);
    showAll = false;
  }

  m_FibersManager.SetFilteringSkipParameters(
    m_Controls.cboSkipPercentage->currentText().toInt() / 100.0,
    m_Controls.spnSkipUpTo->value() * 1000, showAll);
}

void BrainAnalysisView::AnatComboBoxNodeChanged(const mitk::DataNode* node)
{
  if (!node)
  {
    // If the parameter node doesn't exists, the current Anat is deleted
    // and no other anatomic image will be displayed.
    m_Controls.mappers2DSettings->SetAnatGeometry(nullptr);
    m_FibersManager.SetAnat(nullptr, nullptr);
    return;
  }

  auto nonConstNode = m_Controls.cboAnatImage->GetSelectedNode();
  auto im = dynamic_cast<mitk::Image*>(node->GetData());
  m_Controls.mappers2DSettings->SetAnatGeometry(im->GetGeometry());
  m_Controls.regionOfInterestWidget->SetAnat(nonConstNode);
  m_FibersManager.SetAnat(nonConstNode, im);

  // We used to call UpdateAllFibers(false); here but I don't see why we should
  emit RequestUpdateUI();
}

void BrainAnalysisView::DMSelectionChanged(
  const QVector<mitk::DataNode::Pointer>& nodes)
{
  m_SelectedFiberBundles.clear();
  for (const auto& node: nodes)
  {
    if (dynamic_cast<mitk::FiberBundle*>(node->GetData()))
    {
      m_SelectedFiberBundles.push_back(node);
    }
    else if (const auto SO = dynamic_cast<SelectionObject*>(node->GetData()))
    {
      m_Controls.regionOfInterestWidget->UpdateROIInformation(SO);
    }
  }
  DisplayStats();
}

void BrainAnalysisView::NodeAdded(const mitk::DataNode* node)
{
  auto nonConstNode = const_cast<mitk::DataNode*>(node);

  if (m_FibersManager.UpdateGroupIfRequired(nonConstNode))
  {
    m_Controls.regionOfInterestWidget->SetParentNode(m_Groups.ROIs);
    m_FibersManager.GroupAdded(nonConstNode);
    return;
  }

  if (!node->GetData())
  {
    std::cout << "NodeAdded: " << node->GetName() << "\n";
    return;
  }
  std::cout << "NodeAdded: " << node->GetName()
    << " (" << node->GetData()->GetNameOfClass() << ")\n";

  if (auto selectionObject = dynamic_cast<SelectionObject*>(node->GetData()))
  {
    SelectionObjectAdded(nonConstNode, selectionObject);
  }
  else if (mitk::Image* image = dynamic_cast<mitk::Image*>(node->GetData()))
  {
    if (Imeka::Fiber::GetRGBPredicate()->CheckNode(node))
    {
      // nonConstNode->SetMapper(1, Imeka::Mapper::ImekaRGBMapper::New());
      MITK_WARN << "Tried to use mapper\n";
    }
    else if (Imeka::Fiber::GetMaximaPredicate()->CheckNode(node))
    {
      PeaksImageAdded(nonConstNode, image);
    }
  }
  else if (auto fibers =
    dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData()))
  {
    const auto newMax = fibers->GetMaxFiberLength();
    if (newMax > m_Controls.sldMaxFiberLength->maximum())
    {
      SetMaxFibersLength(newMax + 1, false);
    }
  }

  m_FibersManager.NodeAdded(nonConstNode);
}

void BrainAnalysisView::PeaksImageAdded(
  mitk::DataNode* node, mitk::Image* image)
{
  m_Callback.Add("TogglePeaks", false, node,
    [this, image](mitk::DataNode* node)
  {
    bool togglePeaks = false;
    node->GetBoolProperty("TogglePeaks", togglePeaks);
    if (!togglePeaks) { return; }

    node->SetBoolProperty("TogglePeaks", false);

    std::cout << node->GetMapper(1)->GetNameOfClass() << "\n";
    std::cout << node->GetMapper(2)->GetNameOfClass() << "\n";
    const bool isCurrentlyPeaks =
      dynamic_cast<mitk::PeakImageMapper2D*>(node->GetMapper(1)) != nullptr;
    if (isCurrentlyPeaks)
    {
      node->SetBoolProperty("NotAPeaks", true);
      m_Callback.Remove(node, "visible");

      m_MaximaData.Remove(node);
      node->SetMapper(1, mitk::ImageVtkMapper2D::New());
    }
    else
    {
      node->SetBoolProperty("NotAPeaks", false);
      m_Callback.Add("visible", node, [this](mitk::DataNode* node)
      {
        const bool visible = node->IsVisible(nullptr);
        if (!visible) { return; }

        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
      });

      m_MaximaData.Add(node);
      m_MaximaData.AddPeaksMapper(node);
    }
    node->SetVisibility(isCurrentlyPeaks);
  });

  // Call the lambda that we just defined
  node->SetBoolProperty("TogglePeaks", true);
}

void BrainAnalysisView::NodeRemoved(const mitk::DataNode* node)
{
  if (auto deletedfibers =
    dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData()))
  {
    const auto it = std::find(
      m_SelectedFiberBundles.begin(), m_SelectedFiberBundles.end(), node);
    if (it != m_SelectedFiberBundles.end())
    {
      m_SelectedFiberBundles.erase(it);
    }

    const int fMax = deletedfibers->GetRealMaxFiberLength() + 1;
    if (fMax == m_Controls.sldMaxFiberLength->maximum())
    {
      // Deleted fibers had the current maximum length so we must find the new max
      float newMax = 0.0;
      const auto& nodesMap = m_FibersManager.GetNodesMap();
      for (auto it = nodesMap.keyBegin(); it != nodesMap.keyEnd(); ++it)
      {
        auto fibers = dynamic_cast<mitk::FilteredFiberBundle*>((*it)->GetData());
        if (fibers != deletedfibers)
        {
          newMax = std::max(newMax, fibers->GetRealMaxFiberLength());
        }
      }
      SetMaxFibersLength(newMax + 1, true);
    }
  }

  // The node can "not-exist" if the user selects all and delete. Some nodes
  // delete their children, so they would be deleted N times.
  if (!GetDataStorage()->Exists(node)) { return; }

  MITK_INFO << "Removing node: " << node->GetName() << "...\n";
  auto nonConstNode = const_cast<mitk::DataNode*>(node);

  if (!Imeka::Fiber::IsTractGroup(node))
  {
    Nodes children = m_DM.ChildrenOf(node);
    if (!children.empty())
    {
      MITK_INFO << "Also removing " << children.size() << " children nodes.\n";
    }
    for (auto child : children){
      // this line is a precaution, because some nodes have their chidren deleted in FibersManager::NodeRemoded, so they might not exist anymore. 
      if (!GetDataStorage()->Exists(node)) { continue; }
      m_Callback.Remove(child);
      m_FibersManager.NodeRemoved(child);
    }
  }

  m_Callback.Remove(node);

  if (Imeka::Fiber::GetMaximaPredicate()->CheckNode(node))
  {
    m_MaximaData.Remove(node);
  }

  m_FibersManager.NodeRemoved(nonConstNode);
  DisplayStats();
}

void BrainAnalysisView::SetMaxFibersLength(
  const float newMax, const bool deleting)
{
  auto slider = m_Controls.sldMaxFiberLength;
  auto spinbox = m_Controls.spnMaxFiberLength;

  const unsigned int nbFibers = m_DM.GetAll("FilteredFiberBundle").size();
  if (deleting && nbFibers == 0)
  {
    m_Controls.sldMinFiberLength->setMaximum(1);
    m_Controls.spnMinFiberLength->setMaximum(1);
    slider->setMaximum(1);
    spinbox->setMaximum(1);
    return;
  }

  const bool isAtMax = slider->value() == slider->maximum();
  m_Controls.sldMinFiberLength->setMaximum(newMax);
  m_Controls.spnMinFiberLength->setMaximum(newMax);
  slider->setMaximum(newMax);
  spinbox->setMaximum(newMax);

  slider->setSingleStep(newMax / 40);
  slider->setPageStep(newMax / 10);
  spinbox->setSingleStep(newMax / 40);

  if (!deleting && isAtMax)
  {
    slider->setValue(newMax);
  }
}

void BrainAnalysisView::SelectionObjectAdded(
  mitk::DataNode* node,
  SelectionObject* SO)
{
  node->SetBoolProperty("pickable", true);

  typedef Imeka::BoundingObject::InteractorEmitter Interactor;
  auto interactor = dynamic_cast<Interactor*>(
    node->GetDataInteractor().GetPointer());
  if (!interactor)
  {
    // Scene has been loaded from .mitk file. We fake it was added by the
    // widget, so everything is done the right way.
    m_Controls.regionOfInterestWidget->FinishAdding(SO, node, nullptr, false);
    interactor = dynamic_cast<Interactor *>(
      node->GetDataInteractor().GetPointer());
  }
  else
  {
    // SO was created by the user, normally.
    node->SetColor(0.0, 1.0, 0.0);
  }
  connect(interactor, &Interactor::HasBeenChanged, [this](SelectionObject* SO)
  {
    m_FibersManager.SelectionObjectMoved(SO);
  });
  connect(interactor, &Interactor::Unselected,
    &m_FibersManager, &Imeka::Fiber::FibersManager::SelectionObjectReleased);

  m_Controls.regionOfInterestWidget->UpdateROIInformation(SO);

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void BrainAnalysisView::UpdateUI()
{
  const bool atLeastOneSelectionObject =
    m_DM.AtLeastOneVisible("SelectionObject");

  Imeka::Widgets::RegionOfInterestWidget::UI_STATE roiWidget;
  if (atLeastOneSelectionObject)
  {
    roiWidget = Imeka::Widgets::RegionOfInterestWidget::ROI_EXISTS;
  }
  else
  {
    const bool trackingMaskExists =
      m_Controls.cboAnatImage->GetSelectedNode() != nullptr;
    if (!trackingMaskExists)
    {
      roiWidget = Imeka::Widgets::RegionOfInterestWidget::DISABLED;
    }
    else
    {
      roiWidget = Imeka::Widgets::RegionOfInterestWidget::NODE_SELECTED;
    }
  }
  m_Controls.regionOfInterestWidget->UpdateUI(roiWidget);
}

void BrainAnalysisView::DisplayStats()
{
  const auto nodes =
    m_FibersManager.FibersNodesToTransform(m_SelectedFiberBundles);
  if (nodes.empty())
  {
    m_Controls.txtFibersStats->setPlainText("");
    return;
  }

  auto shorter = [](const unsigned int number)
  {
    QString ret = QString::number(number);
    if (number != 0 && (number / 1000) * 1000 == number)
    {
      ret = QString::number(number / 1000) + QString("k");
    }
    return ret;
  };

  QString stats("");
  unsigned int totalNbOfVisibleFibers = 0;
  for (const auto node : nodes)
  {
    bool isVisible = true;
    node->GetBoolProperty("visible", isVisible);
    if (!isVisible) { continue; }

    auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
    QString stddev = "\n";
    if (fiber->GetNumFibers() > 1)
    {
        stddev = QString::number(fiber->GetLengthStDev(), 'f', 1) + " mm\n";
    }
    stats += QString(node->GetName().c_str()) + "\n" +
      "Num. of fibers: " +
      shorter(fiber->GetFibersVisibility().size()) + " / " +
      shorter(fiber->GetNumFibers()) + "\n" +
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
    totalNbOfVisibleFibers += fiber->GetFibersVisibility().size();
  }

  m_Controls.txtFibersStats->setPlainText(stats);
}

void BrainAnalysisView::ExportFibersStatsToCSV()
{
  const auto nodes =
    m_FibersManager.FibersNodesToTransform(m_SelectedFiberBundles);
  if (nodes.empty()) { return; }

  Imeka::Fiber::ExportFibersStatsToCSV(nodes, false);
}
