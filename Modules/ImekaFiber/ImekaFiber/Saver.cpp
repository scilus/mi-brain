#include "Saver.hpp"

#include <mitkIOUtil.h>

#include <QApplication>
#include <QMessageBox>

#include "ImekaCommon/SaveDialog.hpp"
#include "ImekaReport/CSVWriter.hpp"
#include "utils.hpp"

namespace Imeka
{

namespace Fiber
{

const QString Saver::DEFAULT_DIR = "Imeka.BrainAnalysisView.DefaultDir";

Saver::Saver()
  : m_DatasetDataMap(nullptr)
  , m_RTTDataMap(nullptr)
  , m_SaveTo("")
{}

void Saver::AddNodes(NodeDataMap* ndm, const bool isRTT)
{
  if (!isRTT)
  {
    m_DatasetDataMap = ndm;
  }
  else
  {
    m_RTTDataMap = ndm;
  }
}

void Saver::Save(
  const bool inDM,
  Imeka::DataManager& DM)
{
  const auto fibers = FiberBundleFromVisibleStreamlines();
  if (Warn(fibers)) { return; }

  std::string suggestedName = "";
  if (m_DatasetDataMap->size() == 1)
  {
    suggestedName = m_DatasetDataMap->firstKey()->GetName();
  }

  if (inDM)
  {
    SaveToDM(fibers, DM);
  }
  else
  {
    SaveToFile(fibers, suggestedName);
  }
}

void Saver::Save(
  const mitk::DataNode* datasetNode,
  const bool inDM,
  Imeka::DataManager& DM)
{
  const auto fibers = FiberBundleFromVisibleStreamlines(datasetNode, inDM);
  if (Warn(fibers)) { return; }

  if (inDM)
  {
    SaveToDM(fibers, DM);
  }
  else
  {
    SaveToFile(fibers, datasetNode->GetName());
  }
}

void Saver::Save(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode,
  const bool inDM,
  Imeka::DataManager& DM)
{
  const auto fibers = FiberBundleFromVisibleStreamlines(
    datasetNode, TGNode, inDM);
  if (Warn(fibers)) { return; }

  if (inDM)
  {
    SaveToDM(fibers, DM);
  }
  else
  {
    SaveToFile(fibers, TGNode->GetName());
  }
}

bool Saver::Warn(const mitk::FilteredFiberBundle* newFibers) const
{
  if (!newFibers || newFibers->GetNumberOfPoints() == 0)
  {
    QMessageBox::information(
      QApplication::activeWindow(), "Brain Analysis",
      "There's no visible fiber on the screen.");
    return true;
  }
  return false;
}

void Saver::SaveToFile(
  const mitk::FilteredFiberBundle* fibers,
  const std::string& suggestedName)
{
  if (m_SaveTo == "")
  {
    Imeka::FileSystem::SaveDialog saveDialog(nullptr);
    const auto filename = saveDialog.Run(
      DEFAULT_DIR, fibers->Filter(), QString::fromStdString(suggestedName));
    if (filename == "") { return; }

    mitk::IOUtil::Save(fibers, filename.toStdString());
  }
  else
  {
    mitk::IOUtil::Save(fibers, m_SaveTo);
    m_SaveTo = "";
  }
}

void Saver::SaveToDM(
  mitk::FilteredFiberBundle* fibers,
  Imeka::DataManager& DM) const
{
  auto newNode = mitk::DataNode::New();
  newNode->SetName("Fiber bundle");
  newNode->SetData(fibers);
  DM.AddNode(newNode);
}

mitk::FilteredFiberBundle::Pointer
Saver::FiberBundleFromVisibleStreamlines() const
{
  auto newLines = vtkSmartPointer<vtkCellArray>::New();
  auto newPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkUnsignedCharArray> newColors =
    mitk::FilteredFiberBundle::GetNewColorArray(TotalNbPoints());

  unsigned int nbVisible = 0;
  for (auto it = m_DatasetDataMap->constBegin(); it != m_DatasetDataMap->constEnd(); ++it)
  {
    if (it.key()->IsVisible(nullptr))
    {
      nbVisible++;
    }
  }
  if (nbVisible == 1 && (!m_RTTDataMap || m_RTTDataMap->isEmpty()))
  {
    auto& fiberMapperData = m_DatasetDataMap->cbegin()->fiberMapperData;
    auto oldPolydata = fiberMapperData.fiberBundle->GetFiberPolyData();
    auto newPolydata = vtkSmartPointer<vtkPolyData>::New();
    CopyPropertiesSettings(oldPolydata, newPolydata);
    if (fiberMapperData.fiberBundle->IsInOrientationColoring())
    {
      newColors = nullptr;
    }
    fiberMapperData.FiberBundleFromData(
      newLines, newPoints, newColors, newPolydata->GetCellData());
    return NewFiberBundle(newPolydata, newLines, newPoints, newColors);
  }

  const auto end = m_DatasetDataMap->cend();
  for (auto it = m_DatasetDataMap->cbegin(); it != end; ++it)
  {
    if (it.key()->IsVisible(nullptr))
    {
      it.value().fiberMapperData.FiberBundleFromData(
        newLines, newPoints, newColors, nullptr);
    }
  }

  if (m_RTTDataMap)
  {
    const auto end = m_RTTDataMap->cend();
    for (auto it = m_RTTDataMap->cbegin(); it != end; ++it)
    {
      const auto fibers =
        dynamic_cast<mitk::FilteredFiberBundle*>(it.key()->GetData());
      fibers->ExportDataTo(newLines, newPoints, newColors);
    }
  }

  return mitk::FilteredFiberBundle::New(newLines, newPoints, newColors);
}

mitk::FilteredFiberBundle::Pointer
Saver::FiberBundleFromVisibleStreamlines(
  const mitk::DataNode* datasetNode,
  const bool keepColors) const
{
  if (datasetNode->IsVisible(nullptr))
  {
    return (*m_DatasetDataMap)[datasetNode]
      .fiberMapperData.FiberBundleFromData(keepColors, true);
  }
  return nullptr;
}

mitk::FilteredFiberBundle::Pointer
Saver::FiberBundleFromVisibleStreamlines(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode,
  const bool keepColors) const
{
  if (datasetNode->IsVisible(nullptr) && TGNode->IsVisible(nullptr))
  {
    auto& mapperData = (*m_DatasetDataMap)[datasetNode].fiberMapperData;
    return mapperData.partsMapperData[TGNode].FiberBundleFromData(
      mapperData.fiberBundle,
      keepColors || !mapperData.fiberBundle->IsInOrientationColoring(),
      true);
  }
  return nullptr;
}

unsigned int Saver::TotalNbPoints() const
{
  unsigned int totalNbPoints = 0;
  {
    const auto end = m_DatasetDataMap->cend();
    for (auto it = m_DatasetDataMap->cbegin(); it != end; ++it)
    {
      totalNbPoints += it.value().fiberMapperData.TotalNbPoints();
    }
  }

  if (m_RTTDataMap)
  {
    const auto end = m_RTTDataMap->cend();
    for (auto it = m_RTTDataMap->cbegin(); it != end; ++it)
    {
      const auto fibers =
        dynamic_cast<mitk::FilteredFiberBundle*>(it.key()->GetData());
      totalNbPoints += fibers->GetFiberPolyData()->GetNumberOfPoints();
    }
  }

  return totalNbPoints;
}

void ExportFibersStatsToCSV(const ConstNodes& nodes, const bool isRTT)
{
  Imeka::FileSystem::SaveDialog saveDialog(nullptr);
  const QString csvPath = saveDialog.Run(
    Saver::DEFAULT_DIR, Imeka::Report::CSVWriter::FILTER);

  Imeka::Report::CSVWriter csvWriter;
  csvWriter.WriteHeader(QStringList() << "File Name" << "Number of Fibers"
    << "Min. Length (mm)" << "Max. Length (mm)" << "Mean Length (mm)"
    << "Median Length (mm)" << "Standard Deviation (mm)");

  for (const auto node : nodes)
  {
    const auto fiber =
      dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());

    auto line = QStringList() << node->GetName().c_str();
    if (isRTT)
    {
      line << QString::number(fiber->GetNumFibers());
    }
    else
    {
      line << QString::number(fiber->GetFibersVisibility().size()) + "/" +
        QString::number(fiber->GetNumFibers());
    }
    line
      << QString::number(fiber->GetMinFiberLength(), 'f', 1)
      << QString::number(fiber->GetMaxFiberLength(), 'f', 1)
      << QString::number(fiber->GetMeanFiberLength(), 'f', 1)
      << QString::number(fiber->GetMedianFiberLength(), 'f', 1);
    if (fiber->GetNumFibers() > 1)
    {
      line << QString::number(fiber->GetLengthStDev(), 'f', 1);
    }

    csvWriter.WriteHeader(line);
  }

  csvWriter.Save(csvPath.toStdString());
}

} // namespace Fiber

} // namespace Imeka
