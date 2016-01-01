#ifndef VK_API_H
#define VK_API_H

#include <QObject>
#include <QList>

class QNetworkAccessManager;
class QNetworkReply;
class QPixmap;
class VK_api : public QObject
{
    Q_OBJECT
    QNetworkReply* currentDownload;
    QNetworkAccessManager* manager;
    void makeRequest(QUrl &url);
    bool gettingImage;
public:
    explicit VK_api(QObject *parent = 0);
    ~VK_api();
    QString token;
    QString user_id;
    QString xml_reply;
    QPixmap* photo;
    void getFriends();
    void getUser();
    void addFriend(QString id, QString text);
    void addFriend(QString id, QString text, QString cid, QString key);
    void getUserPhoto(QString photo_url);
    void getRequestedFriends();
    //void capcha();
signals:
    void requestReady();
    void requestFailed();
public slots:
    void replyFinished(QNetworkReply* reply);
    void abortReply();
};

#endif // VK_API_H
