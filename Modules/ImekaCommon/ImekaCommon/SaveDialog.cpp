
#include "SaveDialog.hpp"

#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QWidget>

namespace Imeka
{

namespace FileSystem
{

SaveDialog::SaveDialog(QWidget *parent)
  : m_Parent(parent)
  , m_Title("Save")
  , m_SelectedFilter("")
{
}

const QString SaveDialog::Run(
  const QString& settingKey,
  const QString& filter,
  const QString& suggestedName)
{
  QSettings settings;
  QString path = settings.value(settingKey, QDir::homePath()).toString();
  if (!suggestedName.isEmpty())
  {
    path = QDir::cleanPath(path + QDir::separator() + suggestedName);
  }

  QString fileName = QFileDialog::getSaveFileName(
    m_Parent, m_Title, path, filter, &m_SelectedFilter);
  if (fileName != "")
  {
    const QFileInfo fi(fileName);
    if (fi.completeSuffix() == "")
    {
      fileName += ExtensionFromFilter(m_SelectedFilter);
      if (fi.exists())
      {
        // Strange Linux behavior: if user enters "file" and "file.ext" exists
        // Qt getSaveFileName() doesn't ask if he wants to overwrite.
        QMessageBox msgBox(m_Parent);
        msgBox.setText("This file already exists. Do you want to overwrite it?");
        msgBox.addButton("Overwrite", QMessageBox::ActionRole);
        QPushButton *cancel = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(cancel);
        msgBox.exec();
        if (msgBox.clickedButton() == cancel)
        {
          return "";
        }
      }
    }
    if (settingKey != "")
    {
      settings.setValue(settingKey, QFileInfo(fileName).path());
    }
  }

  return fileName;
}

QString SaveDialog::ExtensionFromFilter(const QString &filter)
{
  QString extension;

  const char *extensionRE = "\\*(\\. \\w+)";
  QRegularExpression r(QString::fromLatin1(extensionRE));

  QRegularExpressionMatch match = r.match(filter);
  if (match.hasMatch()) { extension = match.captured(1); }
  return extension.simplified();
}

} // namespace FileSystem

} // namespace Imeka

