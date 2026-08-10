#ifndef IMEKA_FIBER_GROUP_MANAGER_HPP_INCLUDED
#define IMEKA_FIBER_GROUP_MANAGER_HPP_INCLUDED

#include <mitkDataNode.h>

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"

#include "ImekaFiber/Coloring/FibersColors.hpp"
#include "ImekaFiber/Filtering/FilteringUI.hpp"
#include "ImekaFiber/GroupNodes.hpp"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT GroupNodeManager
{
public:
  GroupNodeManager(
    Imeka::Callback& callback,
    Imeka::DataManager& dm,
    FibersColors& fibersColors,
    FilteringUI& filteringUI,
    GroupNodes& groups,
    Callback::CallbackFunction roiCreate);
  ~GroupNodeManager() {}

  void InitializeGroupNode(mitk::DataNode* node);
  bool SetupGroupIfRequired(mitk::DataNode* node);

  void EnsureAllGroupsExist();

private:
  Imeka::Callback& m_Callback;
  Imeka::DataManager& m_DM;
  FibersColors& m_FibersColors;
  FilteringUI& m_FilteringUI;
  GroupNodes& m_Groups;
  Callback::CallbackFunction m_ROICreate;

  void SetupAnatomiesGroup(mitk::DataNode* node);
  void SetupTractsGroup(mitk::DataNode* node);
  void SetupROIsGroup(mitk::DataNode* node);

  bool IsGroupNodePresent(mitk::DataNode* node);
};

} // namespace Fiber
} // namespace Imeka
#endif // IMEKA_FIBER_GROUP_MANAGER_HPP_INCLUDED