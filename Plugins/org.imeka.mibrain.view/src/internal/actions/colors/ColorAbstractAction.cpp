#include "ColorAbstractAction.hpp"

#include <mitkRenderingManager.h>

#include "../utils.hpp"

void ColorAbstractAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  if (NeedAnat())
  {
    Imeka::DataManager DM(m_DS);
    if (!GetAnatNodeWarn(DM, true)) { return; }
  }

  for (const auto& node : selectedNodes)
  {
    node->SetIntProperty("ColorType", GetColoring());
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
