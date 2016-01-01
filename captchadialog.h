#ifndef CAPTCHADIALOG_H
#define CAPTCHADIALOG_H

#include <QDialog>

namespace Ui {
class CaptchaDialog;
}
class QNetworkAccessManager;
class QNetworkReply;
class CaptchaDialog : public QDialog
{
    Q_OBJECT
    QNetworkReply* currentDownload;
    QNetworkAccessManager* manager;
    QPixmap* captchaImg;
public:
    explicit CaptchaDialog(QWidget *parent = 0);
    ~CaptchaDialog();
    QString input;
    void work(QString captcha_img);
private slots:
    void on_buttonBox_accepted();
signals:
    void requestReady();
    void requestFailed();
    void ok();
public slots:
    void replyFinished(QNetworkReply* reply);

private:
    Ui::CaptchaDialog *ui;
};

#endif // CAPTCHADIALOG_H
