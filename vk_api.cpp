#include "vk_api.h"
#include <QNetworkConfigurationManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QPixmap>
VK_api::VK_api(QObject *parent) :
    QObject(parent)
{
    QString token = "";
    QString user_id = "";
    QString lastReply = "";
    gettingImage = false;
    manager = new QNetworkAccessManager(this);
    QNetworkConfigurationManager dc;
    manager->setConfiguration(dc.defaultConfiguration());
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}

VK_api::~VK_api()
{

    if (currentDownload!=NULL)
    {
        currentDownload->abort();
        currentDownload->deleteLater();
    }
    delete manager;
}

void VK_api::getUser()
{
    QUrl url;
    url.setUrl("https://api.vk.com/method/users.get.xml?user_id="+ user_id + "&v=5.14&fields=photo_50&&access_token=" + token);
    makeRequest(url);
}

void VK_api::getFriends()
{
    QUrl url;
    url.setUrl("https://api.vk.com/method/friends.get.xml?user_id="+ user_id + "&v=5.14&access_token=" + token);
    makeRequest(url);
}

void VK_api::getRequestedFriends()
{
    QUrl url;
    url.setUrl("https://api.vk.com/method/friends.getRequests.xml?out=1&v=5.14&access_token=" + token);
    makeRequest(url);
}

void VK_api::getUserPhoto(QString photo_url)
{
    if (token.isEmpty()) return;
    if (user_id.isEmpty()) return;
    if (photo_url.isEmpty()) return;
    QUrl url;
    url.setUrl(photo_url);
    gettingImage = true;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    currentDownload = reply;
}

void VK_api::addFriend(QString id, QString text)
{
    QUrl url;
    QString cleanId = id;
    cleanId.remove("id");
    if (text.isEmpty() == false)
    {
        QString temp = "";
        int max = text.count();
        if (max > 400) max = 400;
        for (int i = 0; i < max; i++)
        {
            temp.append(text.at(i));
        }

        url.setUrl("https://api.vk.com/method/friends.add.xml?user_id="+ cleanId + "&text=" + temp + "&v=5.14&access_token=" + token);
    }
    else
        url.setUrl("https://api.vk.com/method/friends.add.xml?user_id="+ cleanId + "&v=5.14&access_token=" + token);
    makeRequest(url);
}

void VK_api::addFriend(QString id, QString text, QString cid, QString key)
{
    QUrl url;
    QString cleanId = id;
    cleanId.remove("id");
    if (text.isEmpty() == false)
    {
        QString temp = "";
        int max = text.count();
        if (max > 400) max = 400;
        for (int i = 0; i < max; i++)
        {
            temp.append(text.at(i));
        }

        url.setUrl("https://api.vk.com/method/friends.add.xml?user_id="+ cleanId + "&text=" + temp + "&v=5.14&captcha_sid=" + cid + "&captcha_key=" + key + "&access_token=" + token);
    }
    else
        url.setUrl("https://api.vk.com/method/friends.add.xml?user_id="+ cleanId + "&v=5.14&captcha_sid=" + cid + "&captcha_key=" + key + "&access_token=" + token);
    makeRequest(url);
}

void VK_api::makeRequest(QUrl &url)
{
    if (token.isEmpty()) return;
    if (user_id.isEmpty()) return;
    gettingImage = false;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    currentDownload = reply;
}

void VK_api::replyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        if (gettingImage)
        {
            QByteArray bytes = reply->readAll();
            photo = new QPixmap();
            if (photo->loadFromData(bytes) == false)
            {
                emit requestFailed();
                reply->deleteLater();
                currentDownload = NULL;
                return;
            }
        }
        else
        {
            QByteArray bytes = reply->readAll();
            //Reply newReply;
            //newReply.xml = QString::fromUtf8(bytes);
            xml_reply = QString::fromUtf8(bytes);
        }
        emit requestReady();
    }
    else
    {
        emit requestFailed();
    }
    reply->deleteLater();
    currentDownload = NULL;
}

void VK_api::abortReply()
{
    currentDownload->abort();
}
