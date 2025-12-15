
#include "RegionOfInterestWidget.hpp"

#include <QmitkIOUtil.h>

#include "ImekaBoundingObject/NodeUtils.hpp"
#include "ImekaBoundingObject/BoundingObjectFactory.hpp"
#include "ImekaCommon/SaveDialog.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QString>

namespace Imeka
{

namespace Widgets
{

const QString RegionOfInterestWidget::DEFAULT_DIR =
  "Imeka.RegionOfInterest.DefaultDir";

RegionOfInterestWidget::RegionOfInterestWidget(QWidget* parent)
  : QWidget(parent)
  , m_DM(nullptr)
  , m_ParentNode(nullptr)
  , m_Anat(nullptr)
  , m_SelectedBO(nullptr)
  , m_OneBOMax(false)
  , m_IsHelperObject(false)
  , m_AlreadyEmited(false)
{
  setupUi(this);

  connect(this, &RegionOfInterestWidget::RequestUIUpdate,
    this, &RegionOfInterestWidget::UpdateUI);
  connect(this, &RegionOfInterestWidget::RequestROIInformationUpdate,
    this, &RegionOfInterestWidget::UpdateROIInformation);
  connect(this, &RegionOfInterestWidget::DataStorageChanged,
    this, &RegionOfInterestWidget::UpdateBoundingObjectInteractors);
}

void RegionOfInterestWidget::CreateBO(
  const std::string& str, std::function<void(mitk::DataNode*)> f)
{
  auto boundingObject = Imeka::BoundingObject::BoundingObjectFactory::get()
    ->createBoundingObject(str);
  auto newBoundingObjectNode = Imeka::BoundingObject::GetNewBoundingObjectNode(
    boundingObject, m_IsHelperObject);

  if (f)
  {
    f(newBoundingObjectNode);
  }

  FinishAdding(boundingObject, newBoundingObjectNode);
}

void RegionOfInterestWidget::ShowActionButtons()
{
  createROIButton->show();
  importROIButton->show();
  exportROIButton->show();
}

void RegionOfInterestWidget::HideActionButtons()
{
  createROIButton->hide();
  importROIButton->hide();
  exportROIButton->hide();
}

void RegionOfInterestWidget::moveROI(
  const unsigned int idx,
  const double &value)
{
  if (!m_SelectedBO || m_AlreadyEmited) { return; }

  mitk::BaseGeometry* geometry = m_SelectedBO->GetGeometry(GetTime());
  if (!geometry) { return; }

  mitk::Point3D origin = geometry->GetOrigin();
  origin[idx] = value;
  geometry->SetOrigin(origin);

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  emit ROIHasBeenMoved(m_SelectedBO);
}

void RegionOfInterestWidget::resizeROI(
  const unsigned int idx,
  const double &value)
{
  if (!m_SelectedBO || m_AlreadyEmited) { return; }

  mitk::BaseGeometry* geometry = m_SelectedBO->GetGeometry(GetTime());
  if (!geometry) { return; }

  mitk::Vector3D spacing = geometry->GetSpacing();
  spacing[idx] = value / 2.0;
  geometry->SetSpacing(spacing);

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  emit ROIHasBeenResized(m_SelectedBO);
}

void RegionOfInterestWidget::SetDataStorage(
  mitk::DataStorage* storage)
{
  m_DM = Imeka::DataManager(storage);

  emit DataStorageChanged();
}

void RegionOfInterestWidget::SetParentNode(mitk::DataNode::Pointer node)
{
  m_ParentNode = node;

  emit SelectedNodeChanged();
}

void RegionOfInterestWidget::SetAnat(mitk::DataNode::Pointer node)
{
  m_Anat = node->GetData();
}

void RegionOfInterestWidget::UpdateBoundingObjectInteractors()
{
  Imeka::BoundingObject::AddInteractorToAllBoundingObjectNodes(&m_DM);
}

void RegionOfInterestWidget::on_importROIButton_clicked()
{
  if (!m_ParentNode || !m_ParentNode->GetData()) { return; }

  mitk::BoundingObject* boundingObject = nullptr;

  QSettings settings;
  const QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open"), settings.value(DEFAULT_DIR, QDir::homePath()).toString(),
    "Imeka Bounding Object (*.bdo)");
  if (fileName == "") { return; }

  settings.setValue(DEFAULT_DIR, QFileInfo(fileName).path());

  m_ParentNode->UpdateOutputInformation();

  CheckBOExistence();

  auto baseData = mitk::IOUtil::Load(fileName.toStdString())[0];
  boundingObject = dynamic_cast<mitk::BoundingObject*>(baseData.GetPointer());
  if (!boundingObject)
  {
    QMessageBox::warning(QApplication::activeWindow(), tr("Import error"),
      tr("Unable to read the BoundingObject in selected file."));
    return;
  }

  auto newBoundingObjectNode = Imeka::BoundingObject::GetNewBoundingObjectNode(
    boundingObject, m_IsHelperObject);
  FinishAdding(boundingObject, newBoundingObjectNode);
}

void RegionOfInterestWidget::on_exportROIButton_clicked()
{
  if (!m_ParentNode) { return; }

  m_ParentNode->UpdateOutputInformation();
  if (!m_SelectedBO) { return; }

  const auto BONode = m_DM.GetNodeContainingThis(m_SelectedBO);
  QmitkIOUtil::Save(
    m_SelectedBO,
    QString::fromStdString(BONode->GetName()),
    QString(), this);
}

void RegionOfInterestWidget::on_createROIButton_clicked()
{
  // Prompt for ROI creation
  if (!m_ParentNode || !m_ParentNode->GetData())
  {
    QMessageBox::information(
      QApplication::activeWindow(), tr("Creation error"),
      tr("Load and select a node first"));
    return;
  }
  if (!m_Anat)
  {
    QMessageBox::information(
      QApplication::activeWindow(), tr("Creation error"),
      tr("You forgot to set m_BaseData"));
    return;
  }

  CheckBOExistence();

  auto boNode = Imeka::BoundingObject::BoundingObjectFactory::get()
    ->CreateBoundingObjectFromDialog(this, m_IsHelperObject, m_TypesToRemove);
  if (boNode.IsNull()) { return; } // If dialog cancelled

  FinishAdding(
    dynamic_cast<mitk::BoundingObject*>(boNode->GetData()),
    boNode, m_Anat->GetGeometry());
}

void RegionOfInterestWidget::CheckBOExistence()
{
  if (m_OneBOMax && m_SelectedBO)
  {
    const auto boundingObjectNode = m_DM.GetNodeContainingThis(m_SelectedBO);
    m_DM.RemoveNode(boundingObjectNode);
  }
}

void RegionOfInterestWidget::FinishAdding(
  mitk::BoundingObject *boundingObject,
  mitk::DataNode::Pointer boundingObjectNode,
  mitk::BaseGeometry* geometry,
  const bool addToStorage)
{
  typedef Imeka::BoundingObject::InteractorEmitter Interactor;
  auto interactor = Imeka::BoundingObject::AddInteractor(boundingObjectNode);
  connect(interactor.GetPointer(), &Interactor::Selected,
    this, &RegionOfInterestWidget::UpdateROIInformation);
  connect(interactor.GetPointer(), &Interactor::HasBeenChanged,
    this, &RegionOfInterestWidget::UpdateROIInformation);

  if (addToStorage)
  {
    m_SelectedBO = boundingObject;
    emit PreROICreated(boundingObject);
    Imeka::BoundingObject::AddBoundingObjectNodeToParent(
      m_DM.GetDataStorage(), m_ParentNode, boundingObjectNode, geometry);

    if (m_DM.NumberOfVisibleObjects() < 2)
    {
      mitk::RenderingManager::GetInstance()
        ->InitializeViewsByBoundingObjects(m_DM.GetDataStorage());
    }

    emit ROICreated(boundingObject);
  }

  emit RequestROIInformationUpdate(boundingObject);
  emit RequestUIUpdate(ROI_EXISTS);

  m_ParentNode->SetBoolProperty("DisableCallback", true);
  m_ParentNode->SetVisibility(true);
  m_ParentNode->GetPropertyList()->DeleteProperty("DisableCallback");

  mitk::RenderingManager::GetInstance()->InitializeViews();
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void RegionOfInterestWidget::UpdateROIInformation(
  mitk::BoundingObject* boundingObject)
{
  m_AlreadyEmited = true;
  if (boundingObject)
  {
    m_SelectedBO = boundingObject;

    const mitk::BaseGeometry::Pointer geometry =
      boundingObject->GetGeometry(GetTime());
    if (geometry.IsNotNull())
    {
      const mitk::Point3D center = geometry->GetOrigin();
      ROIx->setValue(center[0]);
      ROIy->setValue(center[1]);
      ROIz->setValue(center[2]);

      const mitk::Vector3D spacing = geometry->GetSpacing();
      ROIWidth->setValue(spacing[0] * 2.0);
      ROIHeight->setValue(spacing[1] * 2.0);
      ROIDepth->setValue(spacing[2] * 2.0);

      m_AlreadyEmited = false;
      return;
    }
  }

  ROIx->setValue(0);
  ROIy->setValue(0);
  ROIz->setValue(0);
  ROIWidth->setValue(0);
  ROIHeight->setValue(0);
  ROIDepth->setValue(0);

  m_AlreadyEmited = false;
}

void RegionOfInterestWidget::UpdateUI(const UI_STATE state)
{
  switch(state)
  {
    default:
    case DISABLED:
      createROIButton->setEnabled(false);
      importROIButton->setEnabled(false);
      exportROIButton->setEnabled(false);
      ROIx->setEnabled(false);
      ROIy->setEnabled(false);
      ROIz->setEnabled(false);
      ROIWidth->setEnabled(false);
      ROIHeight->setEnabled(false);
      ROIDepth->setEnabled(false);
      break;

    case NODE_SELECTED:
      createROIButton->setEnabled(true);
      importROIButton->setEnabled(true);
      exportROIButton->setEnabled(false);
      ROIx->setEnabled(false);
      ROIy->setEnabled(false);
      ROIz->setEnabled(false);
      ROIWidth->setEnabled(false);
      ROIHeight->setEnabled(false);
      ROIDepth->setEnabled(false);
      break;

    case ROI_EXISTS:
      createROIButton->setEnabled(true);
      importROIButton->setEnabled(true);
      exportROIButton->setEnabled(true);
      ROIx->setEnabled(true);
      ROIy->setEnabled(true);
      ROIz->setEnabled(true);
      ROIWidth->setEnabled(true);
      ROIHeight->setEnabled(true);
      ROIDepth->setEnabled(true);
      break;
  };

  update();
}

unsigned int RegionOfInterestWidget::GetTime()
{
  return mitk::RenderingManager::GetInstance()->
    GetTimeNavigationController()->GetTime()->GetPos();
}

} // namespace Widgets

} // namespace Imeka
