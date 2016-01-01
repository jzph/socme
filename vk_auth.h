#ifndef VK_AUTH_H
#define VK_AUTH_H

#include <QWidget>
class webWidget;
class QWebView;
class QDateTime;
class QNetworkAccessManager;
class QNetworkReply;
class VK_auth : public QWidget
{
    Q_OBJECT
    QString appID;
    QString permissions;
    QString redir_uri;
    QString version;
    QNetworkReply* currentDownload;
    QNetworkAccessManager* manager;
    webWidget* web;
    QWebView* browser;
public:
    explicit VK_auth(QWidget *parent = 0);
    ~VK_auth();
    bool result;
    QString token;
    QString user_id;
    QDateTime* expires;
signals:
    void successful();
    void failed();
public slots:
    void login();
    void handleBrowser(const QUrl &url);
    void handleWebClosed();
    void replyFinished(QNetworkReply* reply);
};

#endif // VK_AUTH_H
