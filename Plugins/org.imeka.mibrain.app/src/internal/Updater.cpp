
#include "Updater.hpp"

#include <iostream>

#include <QDate>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include "PlitkVersion.hpp"

UpdaterMenu::UpdaterMenu(QWidget* parent)
  : QMenu(parent)
  , m_Reply(nullptr)
{
  this->setVisible(false);
  this->menuAction()->setVisible(false);
  this->setTitle("Update available!");

  const QUrl checkURL(MIBRAIN_VERSION_CHECK);

  auto manager = new QNetworkAccessManager(this);
  QNetworkRequest request(checkURL);
  request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
  connect(manager, &QNetworkAccessManager::finished,
    this, &UpdaterMenu::replyFinished);

  m_Reply = manager->get(request);
  if (!m_Reply)
  {
    std::cout << "Skipping new version check because there's a problem "
                  "with your Internet connetion.\n";
    return;
  }
  const auto errorMethod =
    QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error);
  connect(m_Reply, errorMethod, this, &UpdaterMenu::replyError);
}

void UpdaterMenu::replyFinished(QNetworkReply *reply)
{
  const auto newVersion = QString(reply->readAll()).trimmed();
  std::cout << "Version string: " << newVersion.toStdString() << "\n";
  if (newVersion == "")
  {
    if (QSslSocket::sslLibraryVersionNumber() == 0)
    {
      // I'm not sure why but no internet connection is NOT an error.
      std::cout << "Skipping version check because OpenSSL is not "
        "installed or you are currently offline.\n";
      qInfo() << "Support SSL:  " << QSslSocket::supportsSsl()
        << "\nLib Version Number: " << QSslSocket::sslLibraryVersionNumber()
        << "\nLib Version String: " << QSslSocket::sslLibraryVersionString()
        << "\nLib Build Version Number: " << QSslSocket::sslLibraryBuildVersionNumber()
        << "\nLib Build Version String: " << QSslSocket::sslLibraryBuildVersionString();
    }
    else
    {
      std::cout <<
        "Skipping version check because you are currently offline.\n";
    }
    return;
  }

  QString currentVersion = PLITK_VERSION_NOHASH;
  if (NewerVersion(currentVersion, newVersion))
  {
    this->setVisible(true);
    this->menuAction()->setVisible(true);
    this->setTitle("Update available!");

    auto ask = [this, currentVersion, newVersion]()
    {
      QMessageBox msgBox;
      msgBox.setWindowTitle("MI-Brain");
      msgBox.setText(QString(
        "MI-Brain %1 is available. Do you want to open the Imeka webpage "
        "to download the new version?").arg(newVersion));
      msgBox.setInformativeText(
        QString("You have version %1.").arg(currentVersion));
      msgBox.setStandardButtons(
        QMessageBox::Open | QMessageBox::No | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Open);
      const int answer = msgBox.exec();
      if (answer == QMessageBox::Open)
      {
        const QUrl updateURL(MIBRAIN_VERSION_UPDATE);
        QDesktopServices::openUrl(updateURL);
      }
      if (answer != QMessageBox::Cancel)
      {
        HideAndFree();
      }
    };

#ifdef __APPLE__
    // Dynamically adding a menu doesn't seem to work on macOS so
    // lets use a message box
    ask();
#else
    connect(this, &QMenu::aboutToShow, ask);
#endif
  }
  else
  {
    HideAndFree();
  }
}

bool UpdaterMenu::NewerVersion(
  const QString& currentVersion,
  const QString& newVersion) const
{
  if (newVersion.endsWith(".99")) { return false; }

  const auto currentParts = currentVersion.split(QChar('.'));
  const auto currentDate = QDate(
    currentParts[0].toInt(),
    currentParts[1].toInt(),
    currentParts[2].toInt());
  const auto newtParts = newVersion.split(QChar('.'));
  const auto newDate = QDate(
    newtParts[0].toInt(),
    newtParts[1].toInt(),
    newtParts[2].toInt());
  return newDate > currentDate;
}

void UpdaterMenu::HideAndFree()
{
  this->setVisible(false);
  this->menuAction()->setVisible(false);
  if (m_Reply)
  {
    m_Reply->deleteLater();
    m_Reply = nullptr;
  }
}
