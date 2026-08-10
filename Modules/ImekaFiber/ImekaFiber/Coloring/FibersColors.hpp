
#ifndef IMEKA_FIBER_FIBER_COLORS_HPP_INCLUDED
#define IMEKA_FIBER_FIBER_COLORS_HPP_INCLUDED

#include <QColor>

#include "FiberBundle/FilteredFiberBundle.hpp"

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"
#include "../FiberNodeData.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT FibersColors
{
  typedef mitk::FilteredFiberBundle::Coloring Coloring;

public:
  FibersColors(Imeka::DataManager&, Imeka::Callback&, NodeDataMap&);

  void SetAnat(mitk::DataNode*);
  void FiberDeleted(mitk::DataNode*);
  void SetTractsCategoryActions(mitk::DataNode* node);
  void SetFibersActions(mitk::DataNode* node);

  void SetROIsCategoryActions(mitk::DataNode* node);

  void SetColorsAfterCut(
    const mitk::DataNode* datasetNode,
    const mitk::FilteredFiberBundle* fibers,
    mitk::DataNode* newNode) const;
  void SetColorCoding(mitk::DataNode*, const Coloring) const;

private:
  void DeleteLevelWindowCallbacks(mitk::FilteredFiberBundle*) const;
  void UpdateColorsOfDatasetsColoredByAnat(const Coloring);
  void UpdateForLevelWindow(mitk::FilteredFiberBundle*, const bool) const;
  void UpdateColorCoding(mitk::DataNode*, const Coloring) const;

  Imeka::DataManager& m_DM;
  Imeka::Callback& m_Callback;
  NodeDataMap& m_FibersNodeData;
  mitk::DataNode* m_AnatNode;
};

QColor ImekaFiber_EXPORT GetInitialColor(const mitk::DataNode*);
QColor ImekaFiber_EXPORT GetInitialColor(
  const mitk::DataNode*, const mitk::DataNode*);

template <class C>
QColor GetSingleInitialColor(const C& container)
{
  if (container.size() == 1)
  {
    return GetInitialColor(*container.cbegin());
  }

  std::vector<QColor> colors(container.size());
  std::transform(container.cbegin(), container.cend(), std::begin(colors),
    [](mitk::DataNode* node)
  {
    return GetInitialColor(node);
  });

  if (std::equal(colors.cbegin() + 1, colors.cend(), colors.cbegin()))
  {
    return *colors.cbegin();
  }

  return QColor(255, 255, 255);;
}

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FIBER_COLORS_HPP_INCLUDED
