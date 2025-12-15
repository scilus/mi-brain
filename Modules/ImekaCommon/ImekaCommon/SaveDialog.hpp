
#include "ImekaCommonExports.h"

#include <QString>

class QWidget;

namespace Imeka
{

namespace FileSystem
{

class ImekaCommon_EXPORT SaveDialog
{
public:
  SaveDialog(QWidget *parent = nullptr);

  const QString Run(
    const QString& settingKey,
    const QString& filter,
    const QString& suggestedName = "");

  const QString GetSelectedFilter() const { return m_SelectedFilter; }

private:
  // "Open Blabla (*.bla)" returns ".bla"
  QString ExtensionFromFilter(const QString &);

  QWidget *m_Parent;
  QString m_Title;
  QString m_SelectedFilter;
};

} // namespace FileSystem

} // namespace Imeka

