
#ifndef IMEKA_MIBRAIN_APP_UPDATER_HPP_INCLUDED
#define IMEKA_MIBRAIN_APP_UPDATER_HPP_INCLUDED

#include <QMenu>
#include <QNetworkReply>
#include <QObject>

class UpdaterMenu : public QMenu
{
  Q_OBJECT

public:
  explicit UpdaterMenu(QWidget*);
  ~UpdaterMenu() {}

public slots:
  void replyFinished(QNetworkReply*);
  void replyError(QNetworkReply::NetworkError) { HideAndFree(); }

private:
  bool NewerVersion(const QString&, const QString&) const;
  void HideAndFree();

  QNetworkReply* m_Reply;
};

#endif // IMEKA_MIBRAIN_APP_UPDATER_HPP_INCLUDED
