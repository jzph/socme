#include "vk_auth.h"
#include "webwidget.h"
#include <QWebView>
#include <QDateTime>
#include <QDebug>
#include <QNetworkConfigurationManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
VK_auth::VK_auth(QWidget *parent) :
    QWidget(parent)
{
    appID = "4232245"; // i dont care
    permissions = "friends";
    redir_uri = "https://oauth.vk.com/blank.html";
    version = "5.14";
    web = NULL;
    browser = NULL;
    token = "";
    user_id = "";
    expires = NULL;
    result = false;
    manager = new QNetworkAccessManager(this);
    QNetworkConfigurationManager dc;
    manager->setConfiguration(dc.defaultConfiguration());
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}
VK_auth::~VK_auth()
{
    web->hide();
    web->deleteLater();

    if (currentDownload!=NULL)
    {
        currentDownload->abort();
        currentDownload->deleteLater();
    }
    delete manager;
}

void VK_auth::login()
{
    web = new webWidget(this);
    web->show();
    browser = web->getWebView();
    connect(browser, SIGNAL(urlChanged(QUrl)), this, SLOT(handleBrowser(QUrl)));
    connect(web, SIGNAL(destroyed()), this, SLOT(handleWebClosed()));
    QUrl url;
    QString str = "https://oauth.vk.com/authorize?client_id=" + appID;
    str.append("&scope=" + permissions);
    str.append("&redirect_uri=" + redir_uri);
    str.append("&v=" + version);
    str.append("&display=page");
    str.append("&response_type=token");
    url.setUrl(str);
    browser->load(url);
    return;
}
void VK_auth::handleWebClosed()
{
     if (result == false)
     {
         web->hide();
         web->deleteLater();
         token = "";
         user_id = "";
         emit failed();
     }
}

void VK_auth::handleBrowser(const QUrl &url)
{
    QString str = url.toString();
    if (str.contains("access_token="))
    {
        result = true;
        QStringList first = str.split('#');
        QStringList second = first.last().split('&');
        for (int i = 0; i < second.count(); i++)
        {
            QStringList last = second.at(i).split('=');
            if (last.first() == "access_token")
                token = last.last();
            if (last.first() == "user_id")
                user_id = last.last();
            if (last.first() == "expires_in")
            {
                expires = new QDateTime(QDateTime::currentDateTimeUtc());
                expires->addSecs(last.last().toInt());
            }
        }
        emit successful();
        web->hide();
        web->deleteLater();
        return;
    }
    else if (str.contains("access_denied"))
    {
        web->hide();
        web->deleteLater();
        emit failed();
    }
    return;
}

void VK_auth::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    currentDownload = NULL;
    return;
}
