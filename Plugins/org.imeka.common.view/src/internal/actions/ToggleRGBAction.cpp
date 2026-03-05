
#include "ToggleRGBAction.hpp"

#include "ImekaCommon/RGBMapper.hpp"

void ToggleRGBAction::Run(const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  mitk::Image::Pointer img = nullptr;
  for (const auto& node : selectedNodes)
  {
    img = dynamic_cast<mitk::Image*>(node->GetData());
    if (img->GetDimension() == 4 && img->GetDimension(3) == 3)
    {
      mitk::ImageVtkMapper2D::Pointer mpr;
      if (dynamic_cast<Imeka::Mapper::ImekaRGBMapper*>(node->GetMapper(1)))
      {
        mpr = mitk::ImageVtkMapper2D::New();
      }
      else
      {
        mpr = Imeka::Mapper::ImekaRGBMapper::New();
      }

      node->SetMapper(1, mpr);
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    }
    else
    {
      MITK_WARN << "Could not view " << node->GetName() << " as RGB.\n" <<
        "Make sure the image has 4 dimensions and 3 steps in the last dimension.";
    }
  }
}
