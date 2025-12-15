
#ifndef IMEKA_NODE_PREDICATE_IMAGE_INFO_HPP_INCLUDED
#define IMEKA_NODE_PREDICATE_IMAGE_INFO_HPP_INCLUDED

#include <mitkNodePredicateBase.h>

#include <map>
#include <set>

#include "ImekaCommonExports.h"

namespace mitk
{

class BaseData;

/*
Check if node is a mitk::Image and other things if they were requested,
such as number of dimensions, size of dimension, etc.
*/
class ImekaCommon_EXPORT NodePredicateImageInfo: public NodePredicateBase
{
public:
  mitkClassMacro(NodePredicateImageInfo, NodePredicateBase);
  itkFactorylessNewMacro(Self)
  itkCloneMacro(Self)

  virtual ~NodePredicateImageInfo();

  virtual bool CheckNode(const mitk::DataNode* node) const override;

  itkSetMacro(PixelType, std::string)
  void SetDimension(const unsigned int d)
  {
    m_Dimension = d;
    m_UseDimension = true;
  }

  void SetPixelComponents(const size_t pc)
  {
    m_PixelComponents = pc;
    m_UsePixelComponents = true;
  }

  void AddDimensionSize(const unsigned int d, const unsigned int s)
  {
    m_DimensionSize[d].insert(s);
  }

protected:
  NodePredicateImageInfo();

  std::string m_PixelType;
  unsigned int m_Dimension; bool m_UseDimension;
  size_t m_PixelComponents; bool m_UsePixelComponents;
  std::map<unsigned int, std::set<unsigned int>> m_DimensionSize;
};

} // namespace mitk

#endif // IMEKA_NODE_PREDICATE_IMAGE_INFO_HPP_INCLUDED

