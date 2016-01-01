#include "captchadialog.h"
#include "ui_captchadialog.h"
#include <QNetworkConfigurationManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QStringList>
#include <QPixmap>
CaptchaDialog::CaptchaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaptchaDialog)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    QNetworkConfigurationManager dc;
    manager->setConfiguration(dc.defaultConfiguration());
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
    currentDownload = NULL;
    captchaImg = NULL;
}

CaptchaDialog::~CaptchaDialog()
{
    if (currentDownload != NULL)
        currentDownload->abort();
    delete ui;
    delete manager;
}

void CaptchaDialog::work(QString captcha_img)
{
    QUrl url;
    url.setUrl(captcha_img);
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    currentDownload = reply;
}

void CaptchaDialog::replyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        captchaImg = new QPixmap();
        if (captchaImg->loadFromData(bytes) == false)
        {
            emit requestFailed();
            return;
        }
        ui->label->setPixmap(captchaImg[0]);
    }
    else
    {
        emit requestFailed();
    }
    reply->deleteLater();
    currentDownload = NULL;
}

void CaptchaDialog::on_buttonBox_accepted()
{
    if (captchaImg == NULL) return;
    if (ui->lineEdit->text().isEmpty()) return;
    input = ui->lineEdit->text();
    this->hide();
    emit ok();
}
