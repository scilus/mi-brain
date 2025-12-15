
#include "BinaryImageAction.hpp"

#include <mitkImage.h>

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

  auto itkImage = Imeka::Fiber::GetTractDensityImage<unsigned char>(
    allStreamlines, anatNode, true);

  auto img = mitk::Image::New();
  img->InitializeByItk(itkImage.GetPointer());
  img->SetVolume(itkImage->GetBufferPointer());

  auto node = mitk::DataNode::New();
  node->SetData(img);

  node->SetName("Binary Map");
  DM.AddNode(node);
}
