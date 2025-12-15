
#include "RGBMapper.hpp"

#include <mitkImageAccessByItk.h>
#include <mitkSmartPointerProperty.h>

namespace Imeka
{

namespace Mapper
{

ImekaRGBMapper::ImekaRGBMapper()
  : m_RGBImage(nullptr)
{}

void ImekaRGBMapper::SetDataNode(mitk::DataNode* node)
{
  mitk::ImageVtkMapper2D::SetDataNode(node);
  m_RGBImage = nullptr;
}

const mitk::Image* ImekaRGBMapper::GetInput()
{
  if (!m_RGBImage)
  {
    auto realImage = GetDataNode()->GetData();
    AccessFixedDimensionByItk_1(
      static_cast<const mitk::Image *>(realImage),
      ToRGBImage, 4, m_RGBImage);

    m_RGBImage->GetGeometry()->SetIndexToWorldTransform(
      realImage->GetGeometry()->GetIndexToWorldTransform());

    this->GetDataNode()->SetProperty(
      "ScilPy RGB Image",
      mitk::SmartPointerProperty::New(m_RGBImage));
  }

  return m_RGBImage;
}

} // namespace Mapper

} // namespace Imeka
