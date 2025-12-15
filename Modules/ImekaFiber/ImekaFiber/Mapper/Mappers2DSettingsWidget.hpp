#ifndef IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED
#define IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED

#include <QWidget>

#include "../FiberNodeData.hpp"

#include "ui_Mappers2DSettingsWidget.h"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT Mappers2DSettingsWidget
  : public QWidget, public Ui::Mappers2DSettingsWidget
{
  Q_OBJECT

public:
  // Yes, this is a hack. I need to share this class between 2 plugins.
  static Mappers2DSettingsWidget* Instance;

  static void SetVisibility(mitk::DataNode* node);
  static void SetFiberThickness(mitk::DataNode* node);

  void AddNodes(NodeDataMap*);
  void RemoveNodes(NodeDataMap*);
  bool IsEnabled() { return btn2D->isChecked(); }

  Mappers2DSettingsWidget(QWidget* = nullptr);
  void SetAnatGeometry(const mitk::BaseGeometry*);
  void SetThicknessParameters(const float spacing);

public slots:
  void Modified(const bool checked);

private:
  const mitk::BaseGeometry* m_AnatGeo;
  std::vector<NodeDataMap*> m_Maps;
};

} // namespace Widgets

} // namespace Imeka

#endif //IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED
