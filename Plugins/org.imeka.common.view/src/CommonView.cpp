
#include "CommonView.hpp"

#include <berryISelectionService.h>
#include <berryIWorkbenchPage.h>
#include <berryIWorkbenchWindow.h>
#include <berryPlatform.h>

#include <QmitkStdMultiWidget.h>
#include <QmitkStdMultiWidgetEditor.h>

#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "ImekaBoundingObject/NodeUtils.hpp"

CommonView::CommonView()
  : m_ParentWidget(nullptr)
  , m_DM(GetDataStorage())
{
  berry::IPreferencesService* prefService = berry::Platform::GetPreferencesService();
  berry::IPreferences::Pointer prefs =
    prefService->GetSystemPreferences()->Node("org.mitk.editors.stdmultiwidget");
  prefs->PutInt("crosshair gap size", 0);
}

void CommonView::OnSelectionChanged(
  berry::IWorkbenchPart::Pointer /*source*/,
  const QList<mitk::DataNode::Pointer>& nodes)
{
  DMSelectionChanged(nodes.toVector());
}

void CommonView::CallNodeAddedOnAllNodes()
{
  for (auto node : m_DM.GetAllNodes())
  {
    bool undeletable = false, helperObject = false;
    node->GetBoolProperty("undeletable", undeletable);
    node->GetBoolProperty("helper object", helperObject);
    if (!undeletable && !helperObject)
    {
      NodeAdded(node);
    }
  }
}

QmitkStdMultiWidget* CommonView::GetStdMultiWidget()
{
  for (auto& editor : GetSite()->GetPage()->GetEditors())
  {
    auto stdmwe = dynamic_cast<QmitkStdMultiWidgetEditor*>(editor.GetPointer());
    if (stdmwe)
    {
      return stdmwe->GetStdMultiWidget();
    }
  }
  return nullptr;
}

mitk::PlaneGeometry* CommonView::GetPlaneGeometry(
  const char* axis) const
{
  const mitk::IRenderWindowPart* renderWin = GetRenderWindowPart();
  if (!renderWin) { return nullptr; }

  QmitkRenderWindow* axisWin = renderWin->GetQmitkRenderWindow(axis);
  if (!axisWin) { return nullptr; }

  const mitk::VtkPropRenderer* renderer = axisWin->GetRenderer();
  if (!renderer) { return nullptr; }

  return const_cast<mitk::PlaneGeometry*>(
    renderer->GetCurrentWorldPlaneGeometry());
}

unsigned int CommonView::GetCurrentTimeStep() const
{
  return mitk::RenderingManager::GetInstance()
    ->GetTimeNavigationController()->GetTime()->GetPos();
}

void CommonView::GlobalReinit()
{
  mitk::RenderingManager::GetInstance()
    ->InitializeViewsByBoundingObjects(GetDataStorage(), true);
}

void CommonView::Reinit(const mitk::DataNode* node)
{
  auto geo = node->GetData()->GetTimeGeometry();
  mitk::RenderingManager::GetInstance()
    ->InitializeViews(geo, mitk::RenderingManager::REQUEST_UPDATE_ALL, true);
}

bool CommonView::IsReinited(const mitk::DataNode* node) const
{
  const auto nodeGeo = node->GetData()->GetGeometry();
  const auto nodeBB = nodeGeo->GetBoundingBox();

  auto _3DRenderer = mitk::BaseRenderer::GetInstance(
    mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4"));
  const auto worldGeo =
    _3DRenderer->GetSliceNavigationController()->GetCurrentGeometry3D();
  if (!worldGeo) { return false; }

  const auto worldBB = worldGeo->GetBoundingBox();

  return mitk::Equal(*nodeBB, *worldBB, mitk::eps, false);
}
