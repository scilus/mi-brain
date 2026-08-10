
#include "DensityImageAction.hpp"

#include <mitkImageCast.h>
#include <mitkLevelWindowProperty.h>

#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"
#include "utils.hpp"

void DensityImageAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  Imeka::DataManager DM(m_DS);
  const auto anatNode = GetAnatNodeWarn(DM, true);
  if (!anatNode) { return; }

  mitk::FilteredFiberBundle::Pointer allStreamlines = nullptr;
  if (selectedNodes.size() == 1
    && Imeka::Fiber::IsTractsCategory(selectedNodes[0]))
  {
    const auto nodes = DM.DirectChildrenOf(selectedNodes[0]);
    const ConstNodes cNodes(std::begin(nodes), std::end(nodes));
    allStreamlines = Imeka::Fiber::Union(cNodes);
  }
  else
  {
    allStreamlines = Imeka::Fiber::Union(selectedNodes);
  }

  if (!allStreamlines) { return; }

  allStreamlines->ResampleSpline(1);
  auto itkImage = Imeka::Fiber::GetTractDensityImage<float>(
    allStreamlines, anatNode, false);

  if (itkImage.IsNull()) { return; }

  auto img = mitk::Image::New();
  mitk::CastToMitkImage(itkImage, img);

  auto node = mitk::DataNode::New();
  node->SetData(img);
  node->SetName("Density Map");
  node->SetBoolProperty("binary", false);
  node->SetBoolProperty("outline binary", false);

  mitk::LevelWindow lw;
  lw.SetAuto(img);
  node->SetProperty("levelwindow", mitk::LevelWindowProperty::New(lw));

  DM.AddNode(node);
}
