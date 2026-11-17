#ifndef IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED
#define IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED

#include <QWidget>

#include "../FiberNodeData.hpp"

#include "ImekaFiberExports.h"

namespace Ui { class Mappers2DSettingsWidget; }

namespace Imeka
{

namespace Fiber
{

class IMEKAFIBER_EXPORT Mappers2DSettingsWidget
  : public QWidget
{
  Q_OBJECT

public:
  // Yes, this is a hack. I need to share this class between 2 plugins.
  static Mappers2DSettingsWidget* Instance;

  static void SetVisibility(mitk::DataNode* node);
  static void SetFiberThickness(mitk::DataNode* node);

  void AddNodes(NodeDataMap*);
  void RemoveNodes(NodeDataMap*);
  bool IsEnabled() const;

  Mappers2DSettingsWidget(QWidget* = nullptr);
  ~Mappers2DSettingsWidget() override;
  void SetAnatGeometry(const mitk::BaseGeometry*);
  void SetThicknessParameters(const float spacing);

public slots:
  void Modified(const bool checked);

private:
  ::Ui::Mappers2DSettingsWidget* m_Controls;
  const mitk::BaseGeometry* m_AnatGeo;
  std::vector<NodeDataMap*> m_Maps;
};

} // namespace Widgets

} // namespace Imeka

#endif //IMEKA_FIBER_2D_SETTINGS_WIDGET_HPP_INCLUDED
