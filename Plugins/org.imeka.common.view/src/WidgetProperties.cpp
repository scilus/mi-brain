
#include "WidgetProperties.hpp"

#include <QDoubleSpinBox>
#include <QSpinBox>

#include <mitkDataNode.h>
#include <mitkProperties.h>

#include "ImekaCommon/Property/Property.hpp"

WidgetProperties::WidgetProperties(
  const std::string& viewID)
  : m_ViewID(QString::fromStdString(viewID))
  , m_Node(nullptr)
  , m_CurrentTime(0)
{}

void WidgetProperties::SetParentNode(mitk::DataNode* node)
{
  m_Node = node;
}

void WidgetProperties::Add(
  QDoubleSpinBox* widget,
  const double& defaultValue,
  const bool inTime,
  const bool interpolate)
{
  if (!inTime && interpolate)
  {
    throw std::string("");
  }

  QString key("%1/%2");
  if (inTime) { key += "/%3"; }
  key = key.arg(m_ViewID, widget->objectName());

  m_WidgetData.push_back(WidgetData{
    key, widget, defaultValue, inTime, interpolate, std::vector<double>() });

  connect(widget, SIGNAL(valueChanged(double)),
    this, SLOT(StoreToProperties()));
}

std::vector<double> WidgetProperties::GetValues(
  const QDoubleSpinBox* widget) const
{
  return GetValues(widget->objectName());
}

std::vector<double> WidgetProperties::GetValues(
  const QString& fieldName) const
{
  const auto widgetData = WidgetDataFromFieldName(fieldName);

  std::vector<double> values;
  for (const auto key : KeysFromName(fieldName))
  {
    auto valueFromSetting = m_Settings.value(
      key, widgetData.defaultValue).toDouble();

    const auto SKey = Setted(key);
    if (m_Settings.value(SKey, false).toBool())
    {
      Imeka::Property::Set(m_Node, SKey, true);
    }

    values.push_back(Imeka::Property::Get(
      m_Node, key, valueFromSetting));
  }

  return values;
}

void WidgetProperties::ResetToDefault()
{
  BlockSignals(true);
  for (auto& widgetData : m_WidgetData)
  {
    if (!widgetData.inTime)
    {
      const auto valueFromSetting = m_Settings.value(
        widgetData.GetKey(m_CurrentTime),
        widgetData.defaultValue).toDouble();
      widgetData.widget->setValue(valueFromSetting);
    }
    else
    {
      const auto valueFromSetting = m_Settings.value(
        widgetData.settingsKey,
        widgetData.defaultValue).toDouble();
      widgetData.widget->setValue(valueFromSetting);

      for (const auto key : KeysFromName(widgetData.widget->objectName()))
      {
        Imeka::Property::Set(m_Node, key, valueFromSetting);

        const auto SKey = Setted(key);
        Imeka::Property::Set(
          m_Node, SKey, m_Settings.value(SKey, false).toBool());
      }
    }
//     Commented because PM might regret his decision.
//     auto valueFromSetting = m_Settings.value(
//       widgetData.GetKey(m_CurrentTime), widgetData.defaultValue).toDouble();
//     widgetData.widget->setValue(valueFromSetting);
//
//     if (widgetData.inTime)
//     {
//       for (const auto key : KeysFromName(widgetData.widget->objectName()))
//       {
//         auto valueFromSetting = m_Settings.value(
//           key, widgetData.defaultValue).toDouble();
//         Imeka::Property::Set<mitk::DoubleProperty>(
//           m_Node, key, valueFromSetting);
//
//         const auto SKey = Setted(key);
//         Imeka::Property::Set<mitk::BoolProperty>(
//           m_Node, SKey, m_Settings.value(SKey, false).toBool());
//       }
//     }
  }
  BlockSignals(false);

  StoreToProperties();
}

void WidgetProperties::ResetToFactoryDefault()
{
  BlockSignals(true);
  for (auto& widgetData : m_WidgetData)
  {
    widgetData.widget->setValue(widgetData.defaultValue);
  }
  BlockSignals(false);
}

void WidgetProperties::SetValuesAsDefault()
{
  for (auto& widgetData : m_WidgetData)
  {
    m_Settings.setValue(widgetData.settingsKey, widgetData.widget->value());
  }

//   Commented because PM might regret his decision.
//   for (auto& widgetData : m_WidgetData)
//   {
//     if (!widgetData.inTime)
//     {
//       m_Settings.setValue(widgetData.settingsKey, widgetData.widget->value());
//     }
//     else
//     {
//       const auto values = GetValues(widgetData.widget->objectName());
//       const unsigned int nbTimeSteps = values.size();
//       for (unsigned int t = 0; t < nbTimeSteps; ++t)
//       {
//         const auto key = widgetData.GetKey(t);
//         m_Settings.setValue(key, values[t]);
//         if (WasSetted(key))
//         {
//           m_Settings.setValue(Setted(key), true);
//         }
//       }
//     }
//   }
  m_Settings.sync();
}

void WidgetProperties::LoadFromProperties()
{
  if (!m_Node) { return; }

  if (PropertiesHaveNeverBeenSaved(m_WidgetData[0]))
  {
    ResetToDefault();
  }
  else
  {
    BlockSignals(true);
    for (auto& widgetData : m_WidgetData)
    {
      const auto fromProperty = Imeka::Property::Get(
        m_Node, widgetData.GetKey(m_CurrentTime), widgetData.defaultValue);
      widgetData.widget->setValue(fromProperty);
    }
    BlockSignals(false);
  }
}

void WidgetProperties::StoreToProperties()
{
  if (!m_Node) { return; }

  auto store = [this](const WidgetData& widgetData, const bool setted){
    const QString key = widgetData.GetKey(m_CurrentTime);
    Imeka::Property::Set(m_Node, key, widgetData.widget->value());
    if (widgetData.interpolate)
    {
      if (setted)
      {
        Imeka::Property::Set(m_Node, Setted(key), true);
        Interpolate(widgetData); // Interpolate all other values
      }

      // Give values to all other timestep
      const auto values = GetValues(widgetData.widget->objectName());
      const auto keys = KeysFromName(widgetData.widget->objectName());
      for (unsigned int t = 0; t < values.size(); ++t)
      {
        Imeka::Property::Set(m_Node, keys[t], values[t]);
      }
    }
  };

  if (QObject* obj = sender())
  {
    store(WidgetDataFromFieldName(obj->objectName()), true);
  }
  else
  {
    for (auto& widgetData : m_WidgetData)
    {
      store(widgetData, false);
    }
  }
}

void WidgetProperties::TimeChanged(const unsigned int t)
{
  m_CurrentTime = t;

  BlockSignals(true);
  for (const auto& widgetData : m_WidgetData)
  {
    if (!m_Node)
    {
      widgetData.widget->setValue(widgetData.defaultValue);
    }
    else if (widgetData.inTime)
    {
      // Try to give it the value saved in the properties
      // or the default value if none
      const auto key = widgetData.GetKey(m_CurrentTime);
      auto valueFromSetting = m_Settings.value(
        key, widgetData.defaultValue).toDouble();

      widgetData.widget->setValue(Imeka::Property::Get(
        m_Node, key, valueFromSetting));
    }
  }
  BlockSignals(false);
}

void WidgetProperties::Interpolate(const WidgetData& widgetData)
{
  const auto values = GetValues(widgetData.widget->objectName());

  const unsigned int nbTimeSteps = m_Node->GetData()->GetTimeSteps();
  std::vector<bool> wasSetted(nbTimeSteps);
  for (unsigned int t = 0; t < nbTimeSteps; ++t)
  {
    const auto key = widgetData.GetKey(t);
    wasSetted[t] = WasSetted(key);
  }

  for (unsigned int t = 0; t < nbTimeSteps; ++t)
  {
    const auto key = widgetData.GetKey(t);
    auto prop = dynamic_cast<mitk::DoubleProperty*>(
      m_Node->GetProperty(key.toStdString().c_str()));

    unsigned int left = t;
    for (unsigned int i = 0; i < nbTimeSteps; ++i)
    {
      const unsigned int realIdx = (t - i) % nbTimeSteps;
      if (wasSetted[realIdx])
      {
        left = realIdx;
        break;
      }
    }

    unsigned int right = t;
    for (unsigned int i = 0; i < nbTimeSteps; ++i)
    {
      const unsigned int realIdx = (t + i) % nbTimeSteps;
      if (wasSetted[realIdx])
      {
        right = realIdx;
        break;
      }
    }

    // Return default if there's no setted values
    if (left == t || right == t)
    {
      prop->SetValue(values[t]);
      continue;
    }

    // Return the only setted value if there's only one
    if (left == right) { prop->SetValue(values[left]); continue; }

    // Otherwise, interpolate between the two nearest setted values
    const float leftVal = values[left];
    const float rightVal = values[right];
    const float diff =
      std::max(leftVal, rightVal) - std::min(leftVal, rightVal);
    const unsigned int distance = (right - left) % nbTimeSteps;
    const float step = diff / distance;
    if (leftVal < rightVal)
    {
      prop->SetValue(leftVal + step * (t - left));
    }
    else
    {
      prop->SetValue(leftVal - step * (t - left));
    }
  }
}

const WidgetProperties::WidgetData& WidgetProperties::WidgetDataFromFieldName(
  const QString& name) const
{
  for (auto& widgetData : m_WidgetData)
  {
    if (widgetData.widget->objectName() == name)
    {
      return widgetData;
    }
  }
  throw QString("Bad widget name: %1").arg(name);
}

bool WidgetProperties::WasSetted(const QString& key) const
{
  bool wasSetted = false;
  return m_Node->GetBoolProperty(Setted(key).toStdString().c_str(), wasSetted)
    && wasSetted;
}

bool WidgetProperties::PropertiesHaveNeverBeenSaved(
  const WidgetData& widgetData) const
{
  if (!widgetData.inTime)
  {
    return !Imeka::Property::HasProperty(
      m_Node, widgetData.settingsKey);
  }
  return !Imeka::Property::HasProperty(
    m_Node, widgetData.settingsKey.arg(0u));
}

std::vector<QString> WidgetProperties::KeysFromName(
  const QString& name) const
{
  const unsigned int nbTimeSteps = m_Node->GetData()->GetTimeSteps();
  std::vector<QString> keys(nbTimeSteps);
  const auto& key = WidgetDataFromFieldName(name).settingsKey;
  for (unsigned int t = 0; t < nbTimeSteps; ++t)
  {
    keys[t] = key.arg(t);
  }
  return keys;
}

void WidgetProperties::BlockSignals(const bool b)
{
  for (auto& d : m_WidgetData) { d.widget->blockSignals(b); }
}
