#ifndef IMEKA_FIBER_FIBER_MANAGER_HPP_INCLUDED
#define IMEKA_FIBER_FIBER_MANAGER_HPP_INCLUDED

#include "Coloring/SurfaceColors.hpp"
#include "Filtering/FilteringUI.hpp"
#include "FiberNodeData.hpp"
#include "GroupNodes.hpp"
#include "GroupNodeManager.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT FibersManager : public QObject
{
  Q_OBJECT

signals:
  void DisplayStats();

public:
  FibersManager(Imeka::DataManager&, Imeka::Callback&, GroupNodes&, Callback::CallbackFunction);
  ~FibersManager() {}

  void SetAnat(mitk::DataNode*, mitk::Image*);
  void GroupAdded(mitk::DataNode*);
  void NodeAdded(mitk::DataNode*);
  void NodeRemoved(mitk::DataNode*);

  bool UpdateGroupIfRequired(mitk::DataNode*);

  // For Testing only. Don't make it CONST. We should make it const but it
  // doesn't work and I don't know why.
  const FiberNodeData& GetFiberNodeData(const mitk::DataNode* node)
  {
    return m_FibersNodeData[node];
  }
  // For Testing only.
  const FilteringUI& GetFilteringUI() {
    return m_FilteringUI;
  }
  void WaitForAllFutures() const
  {
    m_Filtering.WaitForAllFutures();
  }

  ConstNodes FibersNodesToTransform(const ConstNodes&) const;
  NodeDataMap& GetNodesMap() { return m_FibersNodeData; }

  void SetFilteringLengths(const double minLength, const double maxLength)
  {
    if (minLength == -1.0)
    {
      m_Filtering.SetMaxLength(maxLength);
    }
    else if (maxLength == -1.0)
    {
      m_Filtering.SetMinLength(minLength);
    }
    else
    {
      m_Filtering.SetLengths(minLength, maxLength);
    }
    UpdateAllFibers();
  }
  void SetFilteringSkipParameters(
    const float skipPercentage,
    const unsigned int upTo,
    const bool showAll)
  {
    m_Filtering.SetSkipParameters(skipPercentage, upTo, showAll);
    UpdateAllFibers();
  }

public slots:
  void SelectionObjectMoved(SelectionObject*, const bool = false);
  void SelectionObjectReleased(SelectionObject*);

  void UpdateAlltats();

private:
  void UpdateAllFibers();
  void ComputeFibersVisibility(const mitk::DataNode*, const bool);
  void SurfaceAdded(mitk::DataNode*, mitk::Surface*);
  void ROIAdded(mitk::DataNode*);
  void FibersAdded(mitk::DataNode*, mitk::FilteredFiberBundle*);
  void RemoveFibersNode(mitk::DataNode*);

  void CutStreamlines(mitk::DataNode*);
  void CutStreamlines(
    const mitk::DataNode*, const mitk::DataNode*, const bool);
  void MirrorFibers(
    mitk::DataNode*, mitk::FilteredFiberBundle*, const unsigned int);
  void ShuffleFibers(mitk::DataNode*, mitk::FilteredFiberBundle*);

  Imeka::DataManager& m_DM;
  Imeka::Callback& m_Callback;
  FibersColors m_FibersColors;
  SurfaceColors m_SurfaceColors;
  GroupNodes& m_Groups;

  mitk::DataNode::Pointer m_AnatNode;
  NodeDataMap m_FibersNodeData;

  Filtering m_Filtering;
  FilteringUI m_FilteringUI;
  GroupNodeManager m_GroupNodeManager;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FIBER_MANAGER_HPP_INCLUDED
