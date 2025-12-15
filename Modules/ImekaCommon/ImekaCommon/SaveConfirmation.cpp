
#include "SaveConfirmation.hpp"

#include <boost/algorithm/string/join.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

namespace Imeka
{

namespace FileSystem
{

SaveConfirmation::SaveConfirmation(QWidget *parent)
  : m_Parent(parent)
    , m_BasePath(".")
{}

void SaveConfirmation::SetBasePath(const QString &path)
{
  if (path == "")
  {
    m_BasePath = ".";
  }
  else
  {
    m_BasePath = path;
  }
}

const QString SaveConfirmation::AskBasePath()
{
  m_BasePath = QFileDialog::getExistingDirectory(
    m_Parent, "Choose Directory",
    QDir::home().absolutePath(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  return m_BasePath;
}

void SaveConfirmation::AddFile(const QString &prefix, const QString &ext)
{
  const FileInfo f =
    {ext, QString("%1/%2.%3").arg(m_BasePath).arg(prefix).arg(ext)};
  m_FileInfos[prefix] = f;
}

bool SaveConfirmation::SearchNextAvailableName()
{
  if (OneFileExists())
  {
    QMessageBox msgBox(m_Parent);
    msgBox.setText("One or more files in  \"" + m_BasePath
      + "\" already exist :\n" + GetErrorMessage());
    msgBox.addButton("Overwrite", QMessageBox::ActionRole);
    QPushButton *rename =
      msgBox.addButton("Rename", QMessageBox::ActionRole);
    QPushButton *cancel = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancel);

    msgBox.exec();
    if (msgBox.clickedButton() == rename)
    {
      Rename();
    }
    else if (msgBox.clickedButton() == cancel)
    {
      return false;
    }
  }
  return true;
}

const QString SaveConfirmation::GetPath(const QString &prefix) const
{
  FileInfos::const_iterator it = m_FileInfos.find(prefix);
  if (it != m_FileInfos.end())
  {
    return it->second.finalName;
  }

  return "";
}

bool SaveConfirmation::OneFileExists() const
{
  for (FileInfos::const_iterator it = m_FileInfos.begin();
    it != m_FileInfos.end(); ++it)
  {
    const FileInfo &f = it->second;
    if (QFile::exists(f.finalName))
    {
      return true;
    }
  }
  return false;
}

void SaveConfirmation::Rename()
{
  unsigned int idx = 1;
  do
  {
    if (idx == 0) { throw NoAvailableNameInFolder(); }

    for (FileInfos::iterator it = m_FileInfos.begin();
      it != m_FileInfos.end(); ++it)
    {
      FileInfo &f = it->second;
      f.finalName = QString("%1/%2 %3.%4")
        .arg(m_BasePath)
        .arg(it->first)
        .arg(idx, 2, 10, QChar('0'))
        .arg(f.ext);
    }
    idx++;
  }
  while (OneFileExists());
}

const QString SaveConfirmation::GetErrorMessage() const
{
  std::vector<std::string> badFiles;
  for (FileInfos::const_iterator it = m_FileInfos.begin();
    it != m_FileInfos.end(); ++it)
  {
    const FileInfo &f = it->second;
    if (QFile::exists(f.finalName))
    {
      badFiles.push_back(
        QFileInfo(f.finalName).fileName().toStdString());
    }
  }
  return QString::fromStdString(boost::algorithm::join(badFiles, ", "));
}

} // namespace FileSystem

} // namespace Imeka
