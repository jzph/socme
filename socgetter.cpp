#include "socgetter.h"
#include <QNetworkConfigurationManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
SocFriendsLoader::SocFriendsLoader(QObject *parent) :
    QObject(parent)
{
    QString url = "";
    QString html = "";
    soc.clear();
    manager = new QNetworkAccessManager(this);
    QNetworkConfigurationManager dc;
    manager->setConfiguration(dc.defaultConfiguration());
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}

SocFriendsLoader::~SocFriendsLoader()
{

    if (currentDownload!=NULL)
    {
        currentDownload->abort();
        currentDownload->deleteLater();
    }
    delete manager;
}

void SocFriendsLoader::setThreadURL(QString url)
{
    this->url = url;
}

void SocFriendsLoader::socMe()
{
    soc.clear();
    if (url.isEmpty()) return;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    currentDownload = reply;
    qDebug() << "downloading";
}

void SocFriendsLoader::replyFinished(QNetworkReply *reply)
{
    qDebug() << reply->error();
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        html = QString::fromUtf8(bytes);
        int pos = -1;
        bool isPost = false;
        bool tag = false;
        QString buffer = "";
        QString currentTag = "";
        while (pos != html.size())
        {
            pos++;
            if (pos == html.size()) break;
            if (html.at(pos) == '<')
            {
                tag = true;
            }
            if (isPost)
            {
                buffer.append(html.at(pos));
            }
            if (tag)
            {
                currentTag.append(html.at(pos));
                if (html.at(pos) == '>')
                {
                    if (currentTag.contains("<blockquote"))
                        isPost = true;
                    if (currentTag == "</blockquote>")
                    {
                        isPost = false;
                        QRegExp regexp("id\\d+");
                        buffer.remove("</blockquote>");

                        if (buffer.contains(regexp))
                        {
                            regexp.indexIn(buffer);
                            QStringList list = regexp.capturedTexts();
                            if (list.isEmpty() == false)
                            {
                                Soc post;
                                post.id = list.first();
                                post.post = buffer;
                                soc.append(post);
                            }
                        }
                        buffer.clear();
                    }
                    tag = false;
                    currentTag.clear();
                }
            }
        }
//        for (int i=0; i <soc.length(); i++)
//        {
//            qDebug() << soc.at(i).post;
//        }
        emit requestReady();
    }
    else
    {
        emit requestFailed();
    }
    reply->deleteLater();
    currentDownload = NULL;
}
