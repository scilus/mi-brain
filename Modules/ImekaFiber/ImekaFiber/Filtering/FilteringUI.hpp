
#ifndef IMEKA_FIBER_FILTERING_UI_HPP_INCLUDED
#define IMEKA_FIBER_FILTERING_UI_HPP_INCLUDED

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"
#include "ImekaFiber/Coloring//FibersColors.hpp"
#include "ImekaFiber/Filtering/ActiveROIsDialogBox.hpp"
#include "ImekaFiber/Filtering/Filtering.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT FilteringUI : public QObject
{
  Q_OBJECT

public:
  static const char* NbActiveROIsPropertyName;

  FilteringUI(
    Imeka::Callback&, Imeka::DataManager&, FibersColors&, Filtering&);

  void LoadingSceneCheck(const mitk::DataNode*);
  bool HasLoadedScene() const { return m_HasLoadedScene; } // For testing only
  bool IsLoadingScene() const { return m_IsLoadingScene; }
  bool FinishedLoading();
  bool AsksMeToStop() const { return m_DontProcessAnythingRelatedToFibers; }
  bool SurfaceNodeUpdated(const mitk::DataNode*, mitk::Surface*);

  void SetROIProperties(mitk::DataNode*) const;
  void AddActionsToTractsCategory(mitk::DataNode*);
  void AddActionsToDataset(mitk::DataNode*);
  mitk::DataNode::Pointer NewActiveROINode(
    const mitk::DataNode*, const TractGroup::Line&);
  mitk::DataNode::Pointer NewTractGroupNode(
    const std::string& = "NewTractGroup") const;

  std::set<mitk::DataNode*> FilteringNodeRemoved(const mitk::DataNode*);

public slots:
  void SetMinLength(const float f) { m_Filtering.SetMinLength(f); }
  void SetMaxLength(const float f) { m_Filtering.SetMaxLength(f); }

signals:
  void RequestUpdateDataset(mitk::DataNode*);
  void RequestGenerateStats();

private:
  void AddActionsToROI(mitk::DataNode*);
  void AddActionsToTractGroup(const bool, mitk::DataNode*, mitk::DataNode*);
  void AddActionsToActiveROI(
    mitk::DataNode*, mitk::DataNode*, mitk::DataNode*);

  bool ManageTractGroupSelection(mitk::DataNode*, mitk::DataNode*);
  mitk::DataNode* GetROILinkedTo(const mitk::DataNode*) const;
  Nodes GetAllSO() const { return m_DM.GetAll("SelectionObject"); }
  Nodes GetAllSurfaces() const { return m_DM.GetAll("Surface"); }
  Nodes GetAllROIs() const;

  Imeka::Callback& m_Callback;
  Imeka::DataManager& m_DM;
  FibersColors& m_Colors;
  Filtering& m_Filtering;

  bool m_HasLoadedScene;
  bool m_IsLoadingScene;
  std::map<mitk::BaseData*, long int> m_ObserverIds;
  bool m_DontProcessAnythingRelatedToFibers;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FILTERING_UI_HPP_INCLUDED
