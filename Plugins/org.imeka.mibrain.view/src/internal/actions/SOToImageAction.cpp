#include "SOToImageAction.hpp"

#include <mitkImage.h>
#include <mitkSurface.h>

#include "ImekaFiber/Surfaces.hpp"
#include "utils.hpp"

void SOToImageAction::Run(const QList<mitk::DataNode::Pointer>& nodes)
{
  Imeka::DataManager DM(m_DS);
  const auto anatNode = GetAnatNodeWarn(DM, false);
  if (!anatNode) { return; }

  const auto anat = dynamic_cast<const mitk::Image*>(anatNode->GetData());
  for (auto node : nodes)
  {
    mitk::Surface::ConstPointer surface =
      dynamic_cast<const mitk::Surface*>(node->GetData());
    mitk::Image::Pointer image =
      Imeka::Surface::SurfaceToImage(anat, surface);

    auto newNode = mitk::DataNode::New();
    newNode->SetName(node->GetName() + " mask");
    newNode->SetData(image);
    DM.AddNode(newNode);
  }
}
