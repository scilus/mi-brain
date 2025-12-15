
#ifndef IMEKA_VIEW_WIDGET_PROPERTIES_HPP_INCLUDED
#define IMEKA_VIEW_WIDGET_PROPERTIES_HPP_INCLUDED

#include <QSettings>
#include <QString>

#include <vector>

#include "org_imeka_common_view_Export.h"

class QDoubleSpinBox;

namespace mitk { class DataNode; }

/*
Class that keep track of the double parameter in the view. It manages the
mitk::Property and the QSetting so that the scene can be saved and loaded, and
the nodes can be selected and unselected. It can manage T values in one widget
and those values can be interpolated.
*/
class COMMONVIEW_EXPORT WidgetProperties : public QObject
{
  Q_OBJECT

  struct WidgetData
  {
    QString settingsKey;
    QDoubleSpinBox* widget;
    double defaultValue;
    bool inTime;
    bool interpolate;

    std::vector<double> m_Defaults;

    QString GetKey(const unsigned int t) const
    {
      QString key = settingsKey;
      if (inTime)
      {
        key = key.arg(t);
      }
      return key;
    }
  };

public:
  explicit WidgetProperties(const std::string&);

  void SetParentNode(mitk::DataNode*);

  void Add(QDoubleSpinBox*, const double&, const bool, const bool);
  std::vector<double> GetValues(const QDoubleSpinBox* widget) const;
  std::vector<double> GetValues(const QString&) const;

public slots:
  void ResetToDefault();
  void ResetToFactoryDefault();
  void SetValuesAsDefault();
  void LoadFromProperties();
  void StoreToProperties();
  void TimeChanged(const unsigned int t);

private:
  const QString m_ViewID;
  QSettings m_Settings;
  mitk::DataNode* m_Node;
  std::vector<WidgetData> m_WidgetData;
  unsigned int m_CurrentTime;

  QString Setted(const QString& key) const { return key + "/Setted"; }
  void Interpolate(const WidgetData& widgetData);
  const WidgetData& WidgetDataFromFieldName(const QString&) const;
  bool PropertiesHaveNeverBeenSaved(const WidgetData&) const;
  bool WasSetted(const QString&) const;
  std::vector<QString> KeysFromName(const QString&) const;
  void BlockSignals(const bool b);
};

#endif // IMEKA_VIEW_WIDGET_PROPERTIES_HPP_INCLUDED
