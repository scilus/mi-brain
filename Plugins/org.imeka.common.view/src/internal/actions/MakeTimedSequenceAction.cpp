
#include "MakeTimedSequenceAction.hpp"

#include <mitkImage.h>
#include <mitkProgressBar.h>
#include <mitkRenderingManager.h>

#include <QMessageBox>
#include <QApplication>

#include "MakeTimedSequenceDialog.hpp"

void MakeTimedSequenceAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  if (selectedNodes.length() < 2)
  {
    QMessageBox msgBox;
    msgBox.setText("You need to select at least two time slices to create a "
      "four dimensions sequence.");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }

  MakeTimedSequenceDialog dlg(QApplication::activeWindow());
  if (dlg.exec() == QDialog::Rejected) { return; }

  QString newname = dlg.getName();
  bool eraseall = dlg.getEraseAll();

  auto geom = mitk::ProportionalTimeGeometry::New();
  geom->Initialize(
    selectedNodes.at(0)->GetData()->GetGeometry(), selectedNodes.count());
  geom->SetFirstTimePoint(0.0);
  geom->SetStepDuration(1.0);
  int max = selectedNodes.count();
  mitk::ProgressBar::GetInstance()->AddStepsToDo(4 * max);

  // Initialize each step's 3D geometry and its time bounds value.
  for (int i = 0; i < max; ++i)
  {
    auto node = selectedNodes.at(i);
    geom->SetTimeStepGeometry(node->GetData()->GetGeometry(), i);
    mitk::ProgressBar::GetInstance()->Progress();
  }

  // The first selected node is used to get pixel type
  // and number of channels.
  mitk::Image* img =
    dynamic_cast<mitk::Image*>(selectedNodes.at(0)->GetData());
  mitk::PixelType pixType = img->GetPixelType();
  int numChannels = img->GetNumberOfChannels();
  mitk::Image::Pointer newData = mitk::Image::New();
  newData->Initialize(pixType, *geom, numChannels, max);

  // Copy each the 3D data of each selected node in its
  // corresponding time step. Depending of the user's choice in the
  // dialog, the node is removed from the datastorage afterwards.
  for (int i = 0; i < max; ++i)
  {
    mitk::Image* image =
      dynamic_cast<mitk::Image*>(selectedNodes.at(i)->GetData());
    void *scalardata = image->GetVtkImageData()->GetScalarPointer();
    newData->SetImportVolume(scalardata, i);

    if (eraseall)
    {
      m_DS->Remove(selectedNodes.at(i));
    }

    mitk::ProgressBar::GetInstance()->Progress(2);
  }

  mitk::DataNode::Pointer newnode = mitk::DataNode::New();
  newnode->Initialize();
  newnode->SetName(newname.toStdString());
  newnode->SetData(newData);
  mitk::LevelWindow lw;
  lw.SetAuto(newData);
  newnode->SetLevelWindow(lw);
  newnode->SetBoolProperty("includeInBoundingBox", true);
  m_DS->Add(newnode);

  mitk::RenderingManager::GetInstance()->InitializeViews(geom);
  mitk::ProgressBar::GetInstance()->Progress(max);
}
