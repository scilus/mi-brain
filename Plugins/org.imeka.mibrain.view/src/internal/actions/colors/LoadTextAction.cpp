#include "LoadTextAction.hpp"

#include <fstream>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include <mitkLookupTable.h>
#include <mitkRenderingManager.h>

#include "FiberBundle/FilteredFiberBundle.hpp"
#include "LoadTextActionDialog.hpp"

const QString LoadTextAction::DEFAULT_DIR = "Imeka.BrainAnalysisView.DefaultDir";

void LoadTextAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto window = QApplication::activeWindow();
  if (selectedNodes.size() > 1)
  {
    QMessageBox::warning(
      window, "MI-Brain", "You must select only one tracts node.");
    return;
  }

  QSettings settings;
  const QString fileName = QFileDialog::getOpenFileName(
    window, "Open", settings.value(DEFAULT_DIR, QDir::homePath()).toString(),
    "Text files (*.txt)");
  settings.setValue(DEFAULT_DIR, QFileInfo(fileName).path());

  auto node = selectedNodes[0];
  auto fibers = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
  const unsigned int nbFibers = fibers->GetNumFibers();

  std::vector<float> loadedValues;
  loadedValues.reserve(nbFibers);

  float val;
  std::ifstream infile(fileName.toStdString().c_str());
  while (infile >> val)
  {
    loadedValues.push_back(val);
  }

  if (nbFibers != loadedValues.size())
  {
    const auto msg = QString(
      "Your txt file contains %1 values and is incompatible with the selected "
      "tracts (%2), which contains %3 streamlines.")
      .arg(loadedValues.size())
      .arg(QString::fromStdString(node->GetName()))
      .arg(nbFibers);
    QMessageBox::warning(QApplication::activeWindow(), "MI-Brain", msg);
    return;
  }

  LoadTextActionDialog d(QApplication::activeWindow(), loadedValues);
  d.exec();

  if (d.result() == QDialog::Accepted)
  {
    fibers->ColorFibersByArray(d.GetArray());
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }
}
