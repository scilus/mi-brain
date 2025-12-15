
#include "FibersColors.hpp"

#include <mitkLevelWindowProperty.h>
#include <mitkRenderingManager.h>

#include "ImekaCommon/Colors.hpp"

namespace Imeka
{

namespace Fiber
{

FibersColors::FibersColors(
  Imeka::DataManager& DM,
  Imeka::Callback& callback,
  NodeDataMap& fibersNodeData)
  : m_DM(DM)
  , m_Callback(callback)
  , m_FibersNodeData(fibersNodeData)
  , m_AnatNode(nullptr)
{}

void FibersColors::SetAnat(mitk::DataNode* node)
{
  m_AnatNode = node;
  if (!node)
  {
    UpdateColorsOfDatasetsColoredByAnat(Coloring::Orientation);
    return;
  }

  UpdateColorsOfDatasetsColoredByAnat(Coloring::FromAnat);

  // If the anatomic image changes, we update the color of all fibers
  // using the COLORCODING_FA_BASED parameter.
  m_Callback.Remove(node, "LookupTable");
  m_Callback.Add("LookupTable", node, [this](mitk::DataNode*)
  {
    UpdateColorsOfDatasetsColoredByAnat(Coloring::FromAnat);
  });
}

void FibersColors::FiberDeleted(mitk::DataNode* node)
{
  DeleteLevelWindowCallbacks(
    dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData()));
}

void FibersColors::DeleteLevelWindowCallbacks(
  mitk::FilteredFiberBundle* fiber) const
{
  if (m_AnatNode && fiber->IsInFromAnatColoring())
  {
    m_Callback.Remove(m_AnatNode, "levelwindow");
    m_Callback.Remove(m_AnatNode, "LevelWindowChangeEnded");
  }
}

void FibersColors::SetTractsCategoryActions(mitk::DataNode* node)
{
  m_Callback.Add("ShuffleColor", 0, node, [this](mitk::DataNode* node)
  {
    int shuffleColor = 0;
    node->GetIntProperty("ShuffleColor", shuffleColor);
    if (!shuffleColor) { return; }

    m_Callback.AllowOneMoreLevelOfCallback();
    Imeka::Color::ShuffleColors(m_DM.DirectChildrenOf(node));

    /* We always auto-reset the group node because
      - it's not actually anything heavy.
      - it's simply modifying some properties on the child nodes, it's their
        job to stop the coloring process.
      - otherwise sometimes it's doing nothing because a child node was
        modified but the group still has the same properties so it won't do
        anything. Example: Shuffle on a child, Local on the group.
    */
    node->SetIntProperty("ShuffleColor", 0);
    node->SetColor(-1.0, -1.0, -1.0);
    node->SetIntProperty("ColorType", -1);
  });

  // Normal MITK property.
  node->SetColor(1.0, 1.0, 1.0);
  m_Callback.Add("color", node, [this](mitk::DataNode* node)
  {
    float rgb[3];
    node->GetColor(rgb);
    if (rgb[0] == -1.0) { return; }

    m_Callback.AllowOneMoreLevelOfCallback();
    for (const auto datasetNode : m_DM.DirectChildrenOf(node))
    {
      datasetNode->SetColor(rgb);
    }

    // See documentation in lambda above
    node->SetIntProperty("ShuffleColor", 0);
    node->SetColor(-1.0, -1.0, -1.0);
    node->SetIntProperty("ColorType", -1);
  });

  m_Callback.Add("ColorType", -1, node, [this](mitk::DataNode* node)
  {
    int colorTypeInt = 0;
    node->GetIntProperty("ColorType", colorTypeInt);
    if (colorTypeInt == -1) { return; }

    m_Callback.AllowOneMoreLevelOfCallback();
    for (const auto datasetNode : m_DM.DirectChildrenOf(node))
    {
      datasetNode->SetIntProperty("ColorType", colorTypeInt);
    }

    // See documentation in lambda above
    node->SetIntProperty("ShuffleColor", 0);
    node->SetColor(-1.0, -1.0, -1.0);
    node->SetIntProperty("ColorType", -1);
  });
}

void FibersColors::SetFibersActions(mitk::DataNode* node)
{
  // Uniform and Shuffle are handled here. This is the normal MITK property.
  m_Callback.Add("color", node, [this](mitk::DataNode* node)
  {
    float rgb[] = { 0.0, 0.0, 0.0 };
    node->GetColor(rgb);
    if (rgb[0] == -1.0) { return; }

    SetColorCoding(node, Coloring::Uniform);

    node->SetIntProperty("ColorType", -1);
  });
  m_Callback.Add("ColorType", -1, node, [this](mitk::DataNode* node)
  {
    int colorTypeInt = 0;
    node->GetIntProperty("ColorType", colorTypeInt);
    if (colorTypeInt == -1) { return; }

    SetColorCoding(node, static_cast<Coloring>(colorTypeInt));

    node->SetColor(-1.0, -1.0, -1.0);
  });
}

void FibersColors::SetColorsAfterCut(
  const mitk::DataNode* datasetNode,
  const mitk::FilteredFiberBundle* fibers,
  mitk::DataNode* newNode) const
{
  if (fibers->IsInUniformColoring())
  {
    float rgb[3];
    datasetNode->GetColor(rgb);
    newNode->SetColor(rgb);
  }
  SetColorCoding(newNode, fibers->GetCurrentColoring());
}

void FibersColors::SetColorCoding(
  mitk::DataNode* node,
  const Coloring coloring) const
{
  UpdateColorCoding(node, coloring);  
}

void FibersColors::UpdateColorCoding(
  mitk::DataNode* node, const Coloring coloring) const
{
  // There's no need to update the MapperData because it will contain the same
  // pointer leading to the same vtkUnsignedCharArray.
  auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
  fiber->ApplyColorCoding(node, m_AnatNode, coloring);

  if (coloring == Coloring::FromAnat)
  {
    DeleteLevelWindowCallbacks(fiber);
    m_Callback.Add("levelwindow", m_AnatNode, [this, fiber](mitk::DataNode*) {
      UpdateForLevelWindow(fiber, true);
    });
    m_Callback.Add("LevelWindowChangeEnded", false, m_AnatNode,
      [this, fiber](mitk::DataNode* anatNode) {
        bool ended = false;
        anatNode->GetBoolProperty("LevelWindowChangeEnded", ended);
        if (ended)
        {
          UpdateForLevelWindow(fiber, false);
        }
      });
  }
  else
  {
    DeleteLevelWindowCallbacks(fiber);
  }

  fiber->GetFiberColors()->Modified();
}

void FibersColors::UpdateForLevelWindow(
  mitk::FilteredFiberBundle* fiber,
  const bool movingLevelWindow) const
{
  bool update = true;
  const auto nbFibers = fiber->GetNumFibers();
  if (movingLevelWindow)
  {
    update = nbFibers <= 5000;
  }
  else
  {
    // TDOO Why do we need this?
    update = nbFibers > 5000;
  }

  if (update)
  {
    fiber->ApplyColorCoding(m_AnatNode, m_AnatNode, Coloring::FromAnat);
    fiber->GetFiberColors()->Modified();
  }
}

void FibersColors::UpdateColorsOfDatasetsColoredByAnat(
  const Coloring coloring)
{
  bool atLeastOne = false;
  const auto end = m_FibersNodeData.cend();
  for (auto it = m_FibersNodeData.cbegin(); it != end; ++it)
  {
    auto node = const_cast<mitk::DataNode*>(it.key());
    const auto fiber =
      dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
    if (fiber->IsInFromAnatColoring())
    {
      UpdateColorCoding(node, coloring);
      atLeastOne = true;
    }
  }
  if (atLeastOne)
  {
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }
}

QColor GetInitialColor(const mitk::DataNode* node)
{
  QColor initial = QColor(255, 255, 255);
  if (auto prop = dynamic_cast<mitk::ColorProperty*>(
    node->GetProperty("color")))
  {
    const auto mitkColor = prop->GetColor();
    if (mitkColor.GetRed() >= 0.0) {
      initial.setRedF(mitkColor.GetRed());
      initial.setBlueF(mitkColor.GetBlue());
      initial.setGreenF(mitkColor.GetGreen());
    }
  }

  return initial;
}

QColor GetInitialColor(
  const mitk::DataNode* datasetNode,
  const mitk::DataNode* TGNode)
{
  QColor initial = QColor(255, 255, 255);
  if (auto prop = dynamic_cast<mitk::ColorProperty*>(
    TGNode->GetProperty("uniformColor")))
  {
    const auto mitkColor = prop->GetColor();
    initial.setRedF(mitkColor.GetRed());
    initial.setBlueF(mitkColor.GetBlue());
    initial.setGreenF(mitkColor.GetGreen());
    return initial;
  }

  return GetInitialColor(datasetNode);
}

} // namespace Fiber

} // namespace Imeka
