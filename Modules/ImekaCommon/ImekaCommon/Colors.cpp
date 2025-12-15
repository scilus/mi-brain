
#include "Colors.hpp"

#include <QTime>

#include <mitkDataNode.h>

namespace Imeka
{

namespace Color
{

std::vector<QColor>
GetNColors(const unsigned int nbColors)
{
  std::vector<QColor> colors;
  colors.reserve(nbColors);

  qsrand(15); // Colors are beautiful with this seed
  double rndInit = qrand();
  qsrand(QTime::currentTime().msec());

  for (unsigned int i = 0; i < nbColors; ++i)
  {
    rndInit += 0.618033988749895; // Golden ratio conjugate
    rndInit = fmod(rndInit, 1.0);

    QColor color;
    color.setHsvF(rndInit, 0.99, 0.99);
    colors.push_back(color.toRgb());
  }

  return colors;
}

void ShuffleColors(const Nodes nodes)
{
  unsigned int i = 0;
  const auto colors = GetNColors(nodes.size());
  for (const auto node : nodes)
  {
    Imeka::Color::Apply(node, colors[i++]);
  }
}

void Apply(mitk::DataNode* node, const QColor& color)
{
  const auto r = color.redF();
  const auto g = color.greenF();
  const auto b = color.blueF();
  node->SetColor(r, g, b);
  if (node->GetProperty("binaryimage.selectedcolor"))
  {
    node->SetProperty(
      "binaryimage.selectedcolor", mitk::ColorProperty::New(r, g, b));
  }
}

} // namespace Color

} // namespace Imeka
