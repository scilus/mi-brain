
#include "BinaryImageAction.hpp"

#include <mitkImageCast.h>
#include <mitkLevelWindowProperty.h>

#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"
#include "utils.hpp"

void BinaryImageAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  Imeka::DataManager DM(m_DS);
  const auto anatNode = GetAnatNodeWarn(DM, true);
  if (!anatNode) { return; }

  mitk::FilteredFiberBundle::Pointer allStreamlines = nullptr;
  if (selectedNodes.size() == 1
    && Imeka::Fiber::IsTractsCategory(selectedNodes[0]))
  {
    const Nodes nodes = DM.DirectChildrenOf(selectedNodes[0]);
    const ConstNodes cNodes(std::begin(nodes), std::end(nodes));
    allStreamlines = Imeka::Fiber::Union(cNodes);
  }
  else
  {
    allStreamlines = Imeka::Fiber::Union(selectedNodes);
  }

  if (!allStreamlines) { return; }

  auto itkImage = Imeka::Fiber::GetTractDensityImage<unsigned char>(
    allStreamlines, anatNode, true);

  if (itkImage.IsNull()) { return; }

  auto img = mitk::Image::New();
  mitk::CastToMitkImage(itkImage, img);

  auto node = mitk::DataNode::New();
  node->SetData(img);
  node->SetBoolProperty("binary", true);
  node->SetBoolProperty("outline binary", false);
  node->SetName("Binary Map");

  mitk::LevelWindow lw;
  lw.SetWindowBounds(0, 1);
  node->SetProperty("levelwindow", mitk::LevelWindowProperty::New(lw));

  DM.AddNode(node);
}
