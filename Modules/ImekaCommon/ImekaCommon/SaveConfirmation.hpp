
#ifndef IMEKA_COMMON_SAVE_CONFIRMATION_HPP_INCLUDED
#define IMEKA_COMMON_SAVE_CONFIRMATION_HPP_INCLUDED

#include <QMap>
#include <QString>

#include "ImekaCommonExports.h"

class QWidget;

namespace Imeka
{

namespace FileSystem
{

class ImekaCommon_EXPORT SaveConfirmation
{
  struct FileInfo
  {
    QString ext;
    QString finalName;
  };

  typedef std::map<QString, FileInfo> FileInfos;

  public:
    class NoAvailableNameInFolder {};

    SaveConfirmation(QWidget *parent = nullptr);

    void SetBasePath(const QString &path);
    const QString AskBasePath();
    void AddFile(const QString &prefix, const QString &ext);

    bool SearchNextAvailableName();
    const QString GetPath(const QString &prefix) const;

  private:
    bool OneFileExists() const;
    void Rename();
    const QString GetErrorMessage() const;

    QWidget *m_Parent;
    FileInfos m_FileInfos;
    QString m_BasePath;
};

} // namespace FileSystem

} // namespace Imeka

#endif // IMEKA_COMMON_SAVE_CONFIRMATION_HPP_INCLUDED
