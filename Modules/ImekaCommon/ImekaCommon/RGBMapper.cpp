
#include "RGBMapper.hpp"

#include <mitkImageAccessByItk.h>

namespace Imeka
{

namespace Mapper
{

ImekaRGBMapper::ImekaRGBMapper()
  : m_RGBImage(nullptr)
  , m_SourceImage(nullptr)
{
  MITK_INFO << "Using ImekaRGBMapper";
}

void ImekaRGBMapper::GenerateDataForRenderer(mitk::BaseRenderer* renderer)
{
  auto* node = this->GetDataNode();
  if (node == nullptr)
  {
    return;
  }

  auto* sourceImage = dynamic_cast<mitk::Image*>(node->GetData());
  if (sourceImage == nullptr)
  {
    return;
  }

  if (sourceImage != m_SourceImage.GetPointer())
  {
    m_SourceImage = sourceImage;
    m_RGBImage = nullptr;
  }

  if (m_RGBImage.IsNull())
  {
    if (sourceImage->GetDimension() != 4 || sourceImage->GetDimension(3) != 3)
    {
      MITK_WARN << "ImekaRGBMapper: source image must be 4D with 3 channels in the last dimension";
      return;
    }

    AccessFixedDimensionByItk_1(sourceImage, ToRGBImage, 4, m_RGBImage);
    if (m_RGBImage.IsNull())
    {
      return;
    }

    if (sourceImage->GetGeometry() && m_RGBImage->GetGeometry())
    {
      m_RGBImage->GetGeometry()->SetIndexToWorldTransform(
        sourceImage->GetGeometry()->GetIndexToWorldTransform());
    }

    if (sourceImage->GetTimeGeometry())
    {
      m_RGBImage->SetTimeGeometry(sourceImage->GetTimeGeometry()->Clone());
    }
  }

  auto* originalData = node->GetData();
  node->SetData(m_RGBImage.GetPointer());
  mitk::ImageVtkMapper2D::GenerateDataForRenderer(renderer);
  // node->SetData(originalData);
  if (m_RGBImage.IsNotNull())
  {
    node->SetData(m_RGBImage);
  }
}

} // namespace Mapper

} // namespace Imeka
