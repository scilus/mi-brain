
#include "BasePropertyUtils.hpp"

#include <mitkColorProperty.h>
#include <mitkProperties.h>
#include <mitkStringProperty.h>

#include <QColor>
#include <QVariant>

namespace Imeka
{

namespace Common
{

QVariant BasePropertyToQVariant(mitk::BaseProperty* baseProp)
{
  QVariant data;
  if (!baseProp) { return data; }

  if (const auto cp = dynamic_cast<const mitk::ColorProperty*>(baseProp))
  {
    const unsigned char ratio = 255;
    mitk::Color color = cp->GetColor();
    data.setValue<QColor>(QColor(
      static_cast<int>(color.GetRed() * ratio),
      static_cast<int>(color.GetGreen() * ratio),
      static_cast<int>(color.GetBlue() * ratio)));
  }
  else if (const auto bp = dynamic_cast<mitk::BoolProperty*>(baseProp))
  {
    data = bp->GetValue() ? Qt::Checked : Qt::Unchecked;
  }
  else if (const auto sp = dynamic_cast<mitk::StringProperty*>(baseProp))
  {
    data.setValue<QString>(QString::fromStdString(sp->GetValue()));
  }
  else if (const auto ip = dynamic_cast<mitk::IntProperty*>(baseProp))
  {
    data.setValue<int>(ip->GetValue());
  }
  else if (const auto fp = dynamic_cast<mitk::FloatProperty*>(baseProp))
  {
    data.setValue<float>(fp->GetValue());
  }
  else
  {
    data.setValue<QString>(QString::fromStdString(
      baseProp->GetValueAsString()));
  }

  return data;
}

} // namespace Common

} // namespace Imeka
