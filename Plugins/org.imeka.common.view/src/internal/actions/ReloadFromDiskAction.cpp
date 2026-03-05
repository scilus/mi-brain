
#include "ReloadFromDiskAction.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>

#include <mitkIOUtil.h>

void ReloadFromDiskAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  std::vector<std::string> paths;
  for (const auto& node: selectedNodes)
  {
    std::string path;
    if (node->GetStringProperty("full_path", path))
    {
      const auto qPath = QString::fromStdString(path);
      QFileInfo fileInfo(qPath);
      if (fileInfo.exists() && fileInfo.isFile())
      {
        paths.push_back(path);
        m_DS->Remove(node);
      }
      else
      {
        const QString msg = QString(
          "Can't reload from disk because the file has moved or you can't "
          "reach it with your current setup. Original path was \"%1\"."
        ).arg(qPath);
        QMessageBox::information(QApplication::activeWindow(), "Imeka", msg);
      }
    }
  }

  if (!paths.empty())
  {
    mitk::IOUtil::Load(paths, *m_DS);
  }
}
