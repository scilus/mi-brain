
#include "CommonWorkbenchWindowAdvisor.hpp"

#include <berryCommandContributionItem.h>
#include <berryCommandContributionItemParameter.h>
#include <berryFileEditorInput.h>
#include <berryIPreferences.h>
#include <berryIPreferencesService.h>
#include <berryIQtStyleManager.h>
#include <berryMenuManager.h>
#include <berryPlatformUI.h>
#include <berryQtPreferences.h>
#include <berryWorkbenchPlugin.h>
#include <internal/berryQtShowViewAction.h>

#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>

#include <mitkBaseRenderer.h>

#include <vtkProperty.h>

#include "internal/ViewPlacerAction.hpp"

struct StdMultiWidgetPartListener : public berry::IPartListener
{
  explicit StdMultiWidgetPartListener(vtkOrientationMarkerWidget* markerWidget)
    : m_MarkerWidget(markerWidget)
    , m_WasDisplayed(false)
  {
    auto _3dView =
      mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4");
    m_MarkerWidget->SetInteractor(_3dView->GetInteractor());
  }

  Events::Types GetPartEventTypes() const override
  {
    return Events::ACTIVATED | Events::VISIBLE;
  }

  void PartActivated(
    const berry::IWorkbenchPartReference::Pointer& partRef) override
  {
    if (ViewsToIgnore(partRef)) { return; }

    // Update only if it changed
    const bool shouldDisplay = IsMIBrainlViews(partRef);
    if (shouldDisplay ^ m_WasDisplayed)
    {
      m_MarkerWidget->SetEnabled(shouldDisplay);
      mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4")->Render();

      m_WasDisplayed = shouldDisplay;
    }
  }

  void PartVisible(
    const berry::IWorkbenchPartReference::Pointer& partRef) override
  {
    PartActivated(partRef);
  }

  /* Editors (views) are also considered a WorkbenchPart and are activated
     when the user first click on one of them. We do not want to change
     anything when this happen. Idem for the DM and navigator. */
  bool ViewsToIgnore(const berry::IWorkbenchPartReference::Pointer& partRef)
  {
    const auto viewID = partRef->GetId();
    return viewID == "org.mitk.editors.stdmultiwidget"
      || viewID == "org.mitk.views.datamanager"
      || viewID == "org.mitk.views.imagenavigator";
  }

  bool IsMIBrainlViews(const berry::IWorkbenchPartReference::Pointer& partRef)
  {
    const auto viewID = partRef->GetId();
    return viewID == "org.imeka.views.brainanalysisview"
      || viewID == "org.imeka.views.brainrttview";
  }

private:
  vtkOrientationMarkerWidget* m_MarkerWidget;
  bool m_WasDisplayed;
};

CommonWorkbenchWindowAdvisor::CommonWorkbenchWindowAdvisor(
  berry::WorkbenchAdvisor* workbenchAdvisor,
  berry::IWorkbenchWindowConfigurer::Pointer configurer)
  : QmitkExtWorkbenchWindowAdvisor(workbenchAdvisor, configurer)
  , m_UseMarkerWidget(false)
  , m_SetViewPlacerHotkeys(false)
  , m_HelpMenu(nullptr)
{}

void CommonWorkbenchWindowAdvisor::PostWindowCreate()
{
  // Change the Qt theme to "Light"
  ctkPluginContext* context = berry::WorkbenchPlugin::GetDefault()->GetPluginContext();
  ctkServiceReference styleManagerRef = context->getServiceReference<berry::IQtStyleManager>();
  auto styleManager = context->getService<berry::IQtStyleManager>(styleManagerRef);
  styleManager->SetStyle(":/org.blueberry.ui.qt/lightstyle.qss");

  // Set all the default preferences for MI-Brain. All preferences are linked to a specific id,
  // that's why it's currently separated in 2 groups.
  auto prefService = berry::WorkbenchPlugin::GetDefault()->GetPreferencesService();

  {
    auto prefs = prefService->GetSystemPreferences()
      ->Node(berry::QtPreferences::QT_STYLES_NODE);
    const bool showCategoryNames = prefs->GetBool(
      berry::QtPreferences::QT_SHOW_TOOLBAR_CATEGORY_NAMES, true);
    if (showCategoryNames)
    {
      prefs->PutBool(berry::QtPreferences::QT_SHOW_TOOLBAR_CATEGORY_NAMES, false);
    }
  }

  {
    auto prefs = prefService->GetSystemPreferences()->Node("/org.mitk.views.datamanager");
    const QString pref = "Call global reinit if node is deleted";
    const bool reinitOnDelete = prefs->GetBool(pref, false);
    if (reinitOnDelete)
    {
      prefs->PutBool(pref, true);
    }
  }

  QmitkExtWorkbenchWindowAdvisor::PostWindowCreate();

  // very bad hack...
  berry::IWorkbenchWindow::Pointer window =
    this->GetWindowConfigurer()->GetWindow();
  auto mainWindow =
    static_cast<QMainWindow*>(window->GetShell()->GetControl());

  if (m_SetViewPlacerHotkeys)
  {
    QMenu* viewMenu = mainWindow->menuBar()->addMenu("&View");
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::LEFT));
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::RIGHT));
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::SUPERIOR));
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::INFERIOR));
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::ANTERIOR));
    viewMenu->addAction(new ViewPlacerAction(ViewPlacerAction::POSTERIOR));
  }

  if (m_UseMarkerWidget)
  {
    m_AnnotatedCube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
    m_AnnotatedCube->SetXPlusFaceText("L");
    m_AnnotatedCube->SetXMinusFaceText("R");
    m_AnnotatedCube->SetYPlusFaceText("P");
    m_AnnotatedCube->SetYMinusFaceText("A");
    m_AnnotatedCube->SetZPlusFaceText("S");
    m_AnnotatedCube->SetZMinusFaceText("I");

    vtkProperty* property = m_AnnotatedCube->GetCubeProperty();
    property->SetColor(0.7, 0.7, 0.7);

    property = m_AnnotatedCube->GetTextEdgesProperty();
    property->SetLineWidth(1);
    property->SetDiffuse(0);
    property->SetAmbient(1);
    property->SetColor(0.1800, 0.2800, 0.2300);

    property = m_AnnotatedCube->GetXPlusFaceProperty();
    property->SetColor(1, 0, 0);
    property->SetInterpolationToFlat();
    property = m_AnnotatedCube->GetXMinusFaceProperty();
    property->SetColor(1, 0, 0);
    property->SetInterpolationToFlat();
    property = m_AnnotatedCube->GetYPlusFaceProperty();
    property->SetColor(0, 1, 0);
    property->SetInterpolationToFlat();
    property = m_AnnotatedCube->GetYMinusFaceProperty();
    property->SetColor(0, 1, 0);
    property->SetInterpolationToFlat();
    property = m_AnnotatedCube->GetZPlusFaceProperty();
    property->SetColor(0, 0, 1);
    property->SetInterpolationToFlat();
    property = m_AnnotatedCube->GetZMinusFaceProperty();
    property->SetColor(0, 0, 1);
    property->SetInterpolationToFlat();

    m_MarkerWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    m_MarkerWidget->SetOrientationMarker(m_AnnotatedCube);
    m_MarkerWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
    m_MarkerWidget->SetInteractor(
      mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4")
      ->GetInteractor());

    m_Listener.reset(new StdMultiWidgetPartListener(m_MarkerWidget));
    GetWindowConfigurer()->GetWindow()->GetActivePage()->AddPartListener(
      m_Listener.data());

    m_MarkerWidget->SetEnabled(1);
  }

  // We modified MITK's code to NOT add the Help menu because it's
  // unmodifiable for some reason. We simply add our own version of it
  // with Help Index and About.
  m_HelpMenu = mainWindow->menuBar()->addMenu("&Help");
  const auto viewRegistry =
    berry::PlatformUI::GetWorkbench()->GetViewRegistry();
  for (const auto viewDescriptor : viewRegistry->GetViews())
  {
    if (viewDescriptor->GetId() == "org.blueberry.views.helpindex")
    {
      // Add the damn Help Index!
      berry::IWorkbenchWindow::Pointer _window(window);
      m_HelpMenu->addAction(
        new berry::QtShowViewAction(_window, viewDescriptor));
      m_HelpMenu->addSeparator();
      break;
    }
  }

  /**
  Add the &About action again.
  THIS CODE IS BAD. The berry classes should NEVER be used outside of MITK's
  code. THIS IS A HACK AND SHOULD BE REMOVED. However, it's here because
  - we remove the standard Help menu because we want to add more menus before
    the Help menu, which should be last
  - "org.blueberry.ui.help.aboutAction" kind-of work but it takes their
    extension point most of the time, so we disable theirs in
    Plugins/org.mitk.gui.qt.ext/plugin.xml
  */
  berry::CommandContributionItemParameter::Pointer command(
    new berry::CommandContributionItemParameter(
      window.GetPointer(), QString(),
      "org.blueberry.ui.help.aboutAction",
      berry::CommandContributionItem::STYLE_PUSH));
  command->icon = QIcon();
  command->label = "&About";
  command->tooltip = QString();
  command->shortcut = QKeySequence();
  berry::CommandContributionItem::Pointer item(
    new berry::CommandContributionItem(command));
  item->Fill(m_HelpMenu, nullptr);
}

void CommonWorkbenchWindowAdvisor::Setup()
{
  ShowViewMenuItem(false);
  ShowNewWindowMenuItem(false);
  ShowClosePerspectiveMenuItem(false);
  SetPerspectiveExcludeList(PerspectivesToExclude());
  SetViewExcludeList(ViewsToExclude());
  ShowViewToolbar(true);
  ShowPerspectiveToolbar(false);
  ShowVersionInfo(false);
  ShowMitkVersionInfo(false);
  ShowMemoryIndicator(true);
}

QList<QString> CommonWorkbenchWindowAdvisor::PerspectivesToExclude()
{
  return QList<QString>();
}

QList<QString> CommonWorkbenchWindowAdvisor::ViewsToExclude()
{
  return QList<QString>()
    << "org.blueberry.views.helpcontents"
    << "org.blueberry.views.helpindex"
    << "org.blueberry.views.helpsearch"
    << "org.blueberry.views.logview"
    << "org.mitk.views.datamanager"
    << "org.mitk.views.modules";
}
