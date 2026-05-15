
#include "Colors.hpp"

#include <QRandomGenerator>
#include <QTime>

#include <mitkDataNode.h>

namespace Imeka
{

namespace Color
{

//creating a global generator onces would make more sense to have a fixed seed
QRandomGenerator generator(15); // Colors are beautiful with this seed

std::vector<QColor>
GetNColors(const unsigned int nbColors)
{
  std::vector<QColor> colors;
  colors.reserve(nbColors);

  // QRandomGenerator generator(15); // Colors are beautiful with this seed
  double rndInit = generator.generateDouble();
  generator.seed(QTime::currentTime().msec());

  for (unsigned int i = 0; i < nbColors; ++i)
  {
    rndInit += 0.618033988749895; // Golden ratio conjugate
    rndInit = fmod(rndInit, 1.0);

    QColor color;
    // Randomize the hue, but keep saturation and value high for vibrant colors
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
