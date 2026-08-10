
#include "Toggle2DAction.hpp"

#include "ImekaFiber/Mapper/Mappers2DSettingsWidget.hpp"
#include "ImekaGeometry/ViewUtils.hpp"

void Toggle2DAction::Run(const QList<mitk::DataNode::Pointer>& selectedNodes)
{
  auto toggler = Imeka::Fiber::Mappers2DSettingsWidget::Instance;
  if (!toggler)
  {
    return;
  }

  const auto renderers = Imeka::View::Get2DRenderers();
  for (auto node : selectedNodes)
  {
    bool visible = true;
    node->GetVisibility(visible, renderers.front(), "visible");
    for (auto renderer : renderers)
    {
      node->SetBoolProperty("visible", !visible, renderer);
    }

    auto fiberBundle = dynamic_cast<mitk::FiberBundle*>(node->GetData());
    if (fiberBundle)
    {
      fiberBundle->RequestUpdate2D();
    }
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
