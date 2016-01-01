#ifndef SOCGETTER_H
#define SOCGETTER_H

#include <QObject>
#include <QList>

class QNetworkAccessManager;
class QNetworkReply;

struct Soc
{
    QString id;
    QString post;
};

class SocFriendsLoader : public QObject
{
    Q_OBJECT
    QNetworkReply* currentDownload;
    QNetworkAccessManager* manager;
    QString url;
    QString html;
public:
    explicit SocFriendsLoader(QObject *parent = 0);
    ~SocFriendsLoader();
    QList<Soc> soc;
    void setThreadURL(QString url);
    void socMe();
signals:
    void requestReady();
    void requestFailed();
public slots:
    void replyFinished(QNetworkReply* reply);
};

#endif // SOCGETTER_H
