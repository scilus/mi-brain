
#include "MIBrainPerspective.hpp"

void MIBrainPerspective::CreateInitialLayout(
  berry::IPageLayout::Pointer layout)
{
  auto editorArea = layout->GetEditorArea();

  layout->AddView("org.mitk.views.datamanager",
    berry::IPageLayout::LEFT, 0.25f, editorArea);
  berry::IViewLayout::Pointer lo =
    layout->GetViewLayout("org.mitk.views.datamanager");
  lo->SetCloseable(false);

  layout->AddView("org.imeka.views.brainanalysisview",
    berry::IPageLayout::RIGHT, 0.6f, editorArea);
  lo = layout->GetViewLayout("org.imeka.views.brainanalysisview");
  lo->SetCloseable(false);

  layout->AddView("org.mitk.views.imagenavigator",
    berry::IPageLayout::BOTTOM, 0.5f, "org.mitk.views.datamanager");
  lo = layout->GetViewLayout("org.mitk.views.imagenavigator");
  lo->SetCloseable(false);
}
