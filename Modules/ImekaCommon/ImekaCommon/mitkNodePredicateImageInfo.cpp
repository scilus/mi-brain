
#include "mitkNodePredicateImageInfo.hpp"

#include <mitkDataNode.h>
#include <mitkImage.h>

namespace mitk
{

NodePredicateImageInfo::NodePredicateImageInfo()
  : m_PixelType("")
  , m_Dimension(0)
  , m_UseDimension(false)
  , m_PixelComponents(0)
  , m_UsePixelComponents(false)
{}

NodePredicateImageInfo::~NodePredicateImageInfo()
{}

bool NodePredicateImageInfo::CheckNode(const mitk::DataNode* node) const
{
  if (!node)
  {
    throw std::invalid_argument("NodePredicateImageInfo: invalid node");
  }

  Image* image = dynamic_cast<Image*>(node->GetData());
  if (!image) { return false; }

  if (m_PixelType != ""
    && image->GetPixelType().GetComponentTypeAsString() != m_PixelType)
  {
    return false;
  }

  if (m_UseDimension && image->GetDimension() != m_Dimension)
  {
    return false;
  }

  if (m_UsePixelComponents
   && image->GetPixelType().GetNumberOfComponents() != m_PixelComponents)
  {
    return false;
  }

  for (const auto& kv : m_DimensionSize)
  {
    const auto& dIdx = kv.first;
    const auto& dSizes = kv.second;
    if (dSizes.find(image->GetDimension(dIdx)) == dSizes.end())
    {
      return false;
    }
  }

  return true;
}

} // namespace mitk
