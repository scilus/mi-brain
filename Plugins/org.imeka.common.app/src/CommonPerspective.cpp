
#include "CommonPerspective.hpp"

void CommonPerspective::CreateInitialLayout(
  berry::IPageLayout::Pointer layout)
{
  layout->AddView(
    "org.mitk.views.datamanager", berry::IPageLayout::LEFT,
    0.3f, layout->GetEditorArea());

  berry::IViewLayout::Pointer lo =
    layout->GetViewLayout("org.mitk.views.datamanager");
  lo->SetCloseable(false);

  layout->AddView(
    "org.mitk.views.imagenavigator", berry::IPageLayout::BOTTOM,
    0.5f, "org.mitk.views.datamanager");
}
