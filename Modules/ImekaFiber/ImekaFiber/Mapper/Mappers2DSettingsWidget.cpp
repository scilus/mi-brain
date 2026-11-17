#include "Mappers2DSettingsWidget.hpp"

#include "ui_Mappers2DSettingsWidget.h"
#include "ImekaGeometry/ViewUtils.hpp"

namespace Imeka
{

namespace Fiber
{

Mappers2DSettingsWidget* Mappers2DSettingsWidget::Instance = nullptr;

Mappers2DSettingsWidget::Mappers2DSettingsWidget(QWidget* parent)
  : QWidget(parent)
  , m_Controls(new ::Ui::Mappers2DSettingsWidget)
  , m_AnatGeo(nullptr)
{
  Instance = this;
  m_Controls->setupUi(this);
  connect(m_Controls->btn2D, &QAbstractButton::toggled,
    this, &Mappers2DSettingsWidget::Modified);

  connect(m_Controls->sldThickness, &QAbstractSlider::valueChanged, [this]()
  {
    m_Controls->spnThickness->blockSignals(true);

    auto spacing = 1.0;
    if (m_AnatGeo)
    {
      spacing = m_AnatGeo->GetSpacing()[0];
    }
    m_Controls->spnThickness->setValue(m_Controls->sldThickness->value() / 40.0 * spacing);
    Modified(m_Controls->btn2D->isChecked());

    m_Controls->spnThickness->blockSignals(false);
  });

  connect(m_Controls->spnThickness,
    QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]()
  {
    m_Controls->sldThickness->blockSignals(true);

    auto spacing = 1.0;
    if (m_AnatGeo)
    {
      spacing = m_AnatGeo->GetSpacing()[0];
    }
    m_Controls->sldThickness->setValue(m_Controls->spnThickness->value() * 40.0 / spacing);
    Modified(m_Controls->btn2D->isChecked());

    m_Controls->sldThickness->blockSignals(false);
  });
}

Mappers2DSettingsWidget::~Mappers2DSettingsWidget() { delete m_Controls; }

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
  const float thickness = (Instance) ? Instance->m_Controls->spnThickness->value() : 0.5;
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

bool Mappers2DSettingsWidget::IsEnabled() const { return m_Controls->btn2D->isChecked(); }

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
  m_Controls->sldThickness->blockSignals(true);
  m_Controls->spnThickness->blockSignals(true);

  const auto spacingFactor = spacing / 40.0;
  m_Controls->spnThickness->setMinimum(m_Controls->sldThickness->minimum() * spacingFactor);
  m_Controls->spnThickness->setMaximum(m_Controls->sldThickness->maximum() * spacingFactor);
  m_Controls->spnThickness->setDecimals(
    (spacingFactor >= 0.01) ? 2 : 3);
  m_Controls->spnThickness->setSingleStep(spacingFactor * 2.0);
  m_Controls->spnThickness->setValue(m_Controls->sldThickness->value() * spacingFactor);

  m_Controls->sldThickness->blockSignals(false);
  m_Controls->spnThickness->blockSignals(false);
}

void Mappers2DSettingsWidget::Modified(const bool checked)
{
  MITK_INFO << "Mappers2DSettingsWidget::Modified(" << checked << ")";
  m_Controls->btn2D->setChecked(checked);
  const auto renderers = Imeka::View::Get2DRenderers();
  for (auto map : m_Maps)
  {
    ForEachFiberNode(*map, [this, checked, renderers](mitk::DataNode* node, FiberNodeData& fiberNodeData)
    {
      MITK_INFO << "Setting visibility for node: " << node->GetName() << " to " << checked;
      float oldThickness = 0.0;
      node->GetFloatProperty("Fiber2DSliceThickness", oldThickness);
      if (oldThickness != m_Controls->spnThickness->value())
      {
        node->SetFloatProperty("Fiber2DSliceThickness", m_Controls->spnThickness->value());
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
