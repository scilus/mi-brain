
#include "Toggle2DAction.hpp"

#include "ImekaFiber/Mapper/Mappers2DSettingsWidget.hpp"

void Toggle2DAction::Run(const QList<mitk::DataNode::Pointer>&)
{
  auto toggler = Imeka::Fiber::Mappers2DSettingsWidget::Instance;
  toggler->Modified(!toggler->IsEnabled());
}
