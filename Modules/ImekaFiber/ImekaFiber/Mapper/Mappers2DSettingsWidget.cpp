#include "Mappers2DSettingsWidget.hpp"

#include "ImekaGeometry/ViewUtils.hpp"

namespace Imeka
{

namespace Fiber
{

Mappers2DSettingsWidget* Mappers2DSettingsWidget::Instance = nullptr;

Mappers2DSettingsWidget::Mappers2DSettingsWidget(QWidget* parent)
  : QWidget(parent)
  , m_AnatGeo(nullptr)
{
  Instance = this;
  setupUi(this);
  connect(btn2D, &QAbstractButton::toggled,
    this, &Mappers2DSettingsWidget::Modified);

  connect(sldThickness, &QAbstractSlider::valueChanged, [this]()
  {
    spnThickness->blockSignals(true);

    auto spacing = 1.0;
    if (m_AnatGeo)
    {
      spacing = m_AnatGeo->GetSpacing()[0];
    }
    spnThickness->setValue(sldThickness->value() / 40.0 * spacing);
    Modified(btn2D->isChecked());

    spnThickness->blockSignals(false);
  });

  connect(spnThickness,
    QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]()
  {
    sldThickness->blockSignals(true);

    auto spacing = 1.0;
    if (m_AnatGeo)
    {
      spacing = m_AnatGeo->GetSpacing()[0];
    }
    sldThickness->setValue(spnThickness->value() * 40.0 / spacing);
    Modified(btn2D->isChecked());

    sldThickness->blockSignals(false);
  });
}

void Mappers2DSettingsWidget::SetVisibility(mitk::DataNode* node)
{
  if (!Instance) { return;  }

  for (auto renderer : Imeka::View::Get2DRenderers())
  {
    node->SetBoolProperty("visible", Instance->IsEnabled(), renderer);
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void Mappers2DSettingsWidget::SetFiberThickness(mitk::DataNode* node)
{
  const float thickness = (Instance) ? Instance->spnThickness->value() : 0.5;
  node->SetFloatProperty("Fiber2DSliceThickness", thickness);
}

void Mappers2DSettingsWidget::AddNodes(NodeDataMap* nodesMap)
{
  auto it = std::find(std::begin(m_Maps), std::end(m_Maps), nodesMap);
  if (it == std::end(m_Maps))
  {
    m_Maps.push_back(nodesMap);
  }
}

void Mappers2DSettingsWidget::RemoveNodes(NodeDataMap* nodesMap)
{
  auto it = std::find(std::begin(m_Maps), std::end(m_Maps), nodesMap);
  if (it != std::end(m_Maps))
  {
    m_Maps.erase(it);
  }
}

void Mappers2DSettingsWidget::SetAnatGeometry(
  const mitk::BaseGeometry* anatGeo)
{
  m_AnatGeo = anatGeo;
  if (!anatGeo)
  {
    SetThicknessParameters(1.0);
  }
  else
  {
    SetThicknessParameters(anatGeo->GetSpacing()[0]);
  }
}

void Mappers2DSettingsWidget::SetThicknessParameters(
  const float spacing)
{
  sldThickness->blockSignals(true);
  spnThickness->blockSignals(true);

  const auto spacingFactor = spacing / 40.0;
  spnThickness->setMinimum(sldThickness->minimum() * spacingFactor);
  spnThickness->setMaximum(sldThickness->maximum() * spacingFactor);
  spnThickness->setDecimals(
    (spacingFactor >= 0.01) ? 2 : 3);
  spnThickness->setSingleStep(spacingFactor * 2.0);
  spnThickness->setValue(sldThickness->value() * spacingFactor);

  sldThickness->blockSignals(false);
  spnThickness->blockSignals(false);
}

void Mappers2DSettingsWidget::Modified(const bool checked)
{
  MITK_INFO << "Mappers2DSettingsWidget::Modified(" << checked << ")";
  btn2D->setChecked(checked);
  const auto renderers = Imeka::View::Get2DRenderers();
  for (auto map : m_Maps)
  {
    ForEachFiberNode(*map, [this, checked, renderers](mitk::DataNode* node, FiberNodeData& fiberNodeData)
    {
      MITK_INFO << "Setting visibility for node: " << node->GetName() << " to " << checked;
      float oldThickness = 0.0;
      node->GetFloatProperty("Fiber2DSliceThickness", oldThickness);
      if (oldThickness != spnThickness->value())
      {
        node->SetFloatProperty("Fiber2DSliceThickness", spnThickness->value());
      }

      for (auto renderer : renderers)
      {
        node->SetBoolProperty("visible", checked, renderer);
      }

      if (fiberNodeData.fiberMapper2D.IsNotNull())
      {
        auto fiberBundle = dynamic_cast<mitk::FiberBundle*>(node->GetData());
        if (fiberBundle)
        {
          fiberBundle->RequestUpdate2D();
        }
      }
    });
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

} // namespace Widgets

} // namespace Imeka
