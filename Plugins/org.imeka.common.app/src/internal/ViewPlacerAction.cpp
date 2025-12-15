
#include "ViewPlacerAction.hpp"

#include <mitkBaseRenderer.h>
#include <mitkCameraController.h>

#include "ImekaGeometry/ViewUtils.hpp"

ViewPlacerAction::ViewPlacerAction(
  const ViewPosition viewPosition,
  QWidget* parent)
  : QAction(parent)
  , m_ViewPosition(viewPosition)
{
  switch (m_ViewPosition)
  {
  case LEFT:
    this->setText("Left");
    this->setShortcut(QKeySequence(Qt::Key_L));
    break;
  case RIGHT:
    this->setText("Right");
    this->setShortcut(QKeySequence(Qt::Key_R));
    break;
  case SUPERIOR:
    this->setText("Superior");
    this->setShortcut(QKeySequence(Qt::Key_S));
    break;
  case INFERIOR:
    this->setText("Inferior");
    this->setShortcut(QKeySequence(Qt::Key_I));
    break;
  case ANTERIOR:
    this->setText("Anterior");
    this->setShortcut(QKeySequence(Qt::Key_A));
    break;
  case POSTERIOR:
    this->setText("Posterior");
    this->setShortcut(QKeySequence(Qt::Key_P));
    break;
  }

  connect(this, SIGNAL(triggered()), this, SLOT(Run()));
}

void ViewPlacerAction::Run()
{
  auto renderer = Imeka::View::Get3DRenderer();
  auto cameraController = renderer->GetCameraController();
  if (!cameraController) { return; }

  switch (m_ViewPosition)
  {
  case LEFT:
    cameraController->SetViewToSinister();
    break;
  case RIGHT:
    cameraController->SetViewToDexter();
    break;
  case SUPERIOR:
    cameraController->SetViewToCranial();
    break;
  case INFERIOR:
    cameraController->SetViewToCaudal();
    break;
  case ANTERIOR:
    cameraController->SetViewToAnterior();
    break;
  case POSTERIOR:
    cameraController->SetViewToPosterior();
    break;
  }
}
