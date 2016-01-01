#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vk_auth.h"
#include "vk_api.h"
#include "socgetter.h"
#include "captchadialog.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QStringListModel>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auth = NULL;
    api = NULL;
    SFL = NULL;
    CD = NULL;
    VKLOGIN = false;
    alreadyInFriends = 0;
    addedCount = 0;
    token = "";
    user_id = "";
    selectedId = "";
    captchaId = "";
    captchaKey = "";
    selectedWhenCaptchaAppears = "";
    addingFriend = false;
    gettingSoc = false;
    friends.clear();
    reqFriends.clear();
    SOC.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_vk_login_clicked()
{
    if (VKLOGIN) return;
    if (auth != NULL) return;
    auth = new VK_auth();
    auth->login();
    connect(auth, SIGNAL(successful()), this, SLOT(handleLogin()));
    connect(auth, SIGNAL(failed()), this, SLOT(handleLoginFailed()));
}

void MainWindow::handleLogin()
{
    VKLOGIN = true;
    token = auth->token;
    user_id = auth->user_id;
    user_id.prepend("id");
    api = new VK_api();
    api->token = auth->token;
    api->user_id = auth->user_id;
    api->getUser();
    connect(api, SIGNAL(requestReady()), this, SLOT(handleApiUser()));
}

void MainWindow::handleLoginFailed()
{
    delete auth;
    auth = NULL;
}

void MainWindow::handleApiUser()
{
    if (api == NULL) return;
    ui->vk_login->hide();
    ui->label_3->setEnabled(true);
    ui->thread_url->setEnabled(true);
    ui->soc_thread->setEnabled(true);
    QString photo_url = "";
    QXmlStreamReader *xml = new QXmlStreamReader(api->xml_reply);
    while (!xml->atEnd())
    {
        xml->readNext();
        if (xml->isStartElement())
        {
            if (xml->name() == "first_name")
                first_name = xml->readElementText();
            if (xml->name() == "last_name")
                last_name = xml->readElementText();
            if (xml->name() == "photo_50")
                photo_url = xml->readElementText();
        }
    }
    delete xml;
    ui->vk_name->setText(first_name + " " + last_name);
    disconnect(api, 0, this, 0);
    connect(api, SIGNAL(requestReady()), this, SLOT(handleApiUserPhoto()));
    api->getUserPhoto(photo_url);
}

void MainWindow::handleApiUserPhoto()
{
    if (api == NULL) return;
    ui->vk_avatar->setPixmap(*(api->photo));
    disconnect(api, 0, this, 0);
    connect(api, SIGNAL(requestReady()), this, SLOT(handleFriends()));
    api->getFriends();
}

void MainWindow::handleFriends()
{
    if (api == NULL) return;
    api->xml_reply.replace("<user_id>", "<user_id>id");
    QXmlStreamReader *xml2 = new QXmlStreamReader(api->xml_reply);
    while (!xml2->atEnd())
    {
        xml2->readNext();
        if (xml2->isStartElement())
        {
            if (xml2->name() == "user_id")
            {
                QString temp = xml2->readElementText();
                friends.append(temp);
            }
        }
    }
    delete xml2;
}

void MainWindow::handleRequestedFriends()
{
    if (api == NULL) return;
    reqFriends.clear();
    api->xml_reply.replace("<user_id>", "<user_id>id");
    QXmlStreamReader *xml = new QXmlStreamReader(api->xml_reply);
    while (!xml->atEnd())
    {
        xml->readNext();
        if (xml->isStartElement())
        {
            if (xml->name() == "user_id")
            {
                QString temp = xml->readElementText();
                reqFriends.append(temp);
            }
        }
    }
    delete xml;
    SFL->setThreadURL(ui->thread_url->text());
    connect(SFL, SIGNAL(requestReady()), this, SLOT(handleSFL()));
    SFL->socMe();
    gettingSoc = true;
}

void MainWindow::on_soc_thread_clicked()
{
    if (VKLOGIN == false) return;
    if (gettingSoc) return;
    if (ui->thread_url->text().isEmpty()) return;
    if (ui->thread_url->text().contains("http://") == false)
    {
        ui->thread_url->setText("http://" + ui->thread_url->text());
    }
    if (addingFriend) return;
    gettingSoc = true;
    if (SFL == NULL)
    {
        SFL = new SocFriendsLoader();
    }
    alreadyInFriends = 0;
    addedCount = 0;
    selectedId = "";
    SOC.clear();
    updateList();
    disconnect(api, 0, this, 0);
    connect(api, SIGNAL(requestReady()), this, SLOT(handleRequestedFriends()));
    api->getRequestedFriends();
    ui->result_label->setText("");
    ui->post_text->setEnabled(true);
    ui->add_friend->setEnabled(true);
    ui->id_list->setEnabled(true);
    ui->send_text->setEnabled(true);
    ui->checkBox->setEnabled(true);
    ui->checkBox_2->setEnabled(true);
}

void MainWindow::handleSFL()
{
    if (SFL == NULL)
    {
        return;
    }
    alreadyInFriends = 0;
    addedCount = 0;
    SOC.clear();
    for (int i = 0; i < SFL->soc.count(); i++)
    {
        if (SFL->soc.at(i).id == user_id) continue;
        bool flag = false;
        for (int j = 0; j< SOC.count(); j++)
        {
            if (SOC.at(j)->id == SFL->soc.at(i).id)
                flag = true;
        }
        if (flag) continue;
        SocFriend *f = new SocFriend();
        f->id = SFL->soc.at(i).id;
        f->post = SFL->soc.at(i).post;
        if (reqFriends.contains(SFL->soc.at(i).id))
        {
            addedCount++;
            f->added = true;
        }
        else
            f->added = false;
        f->message = "";
        if (friends.contains(SFL->soc.at(i).id))
        {
            alreadyInFriends++;
            f->inFriends = true;
        }
        else
            f->inFriends = false;
        SOC.append(f);
    }
    updateInfoText();
    updateList();
    connect(ui->id_list, SIGNAL(activated(QModelIndex)), this, SLOT(handleList(QModelIndex)));
    gettingSoc = false;
}

void MainWindow::updateInfoText()
{
    QString str = "Найдено: ";
    str.append(QString::number(SOC.count()) + ". ");
    str.append("Из них уже в друзьях: " + QString::number(alreadyInFriends) + ". ");
    str.append("Заявок отправлено: " + QString::number(addedCount) + ".");
    ui->result_label->setText(str);
}

void MainWindow::updateList()
{
    QStringList* stringList = new QStringList();

    for (int i = 0; i < SOC.count(); i++)
    {
        if (SOC.at(i)->inFriends == false)
        {
            QString temp = SOC.at(i)->id;
            if (SOC.at(i)->added) temp.prepend("(added) ");
            *(stringList) << temp;
        }
    }
    QStringListModel* listModel = new QStringListModel(*stringList, NULL);
    ui->id_list->setModel(listModel);
}

void MainWindow::handleList(const QModelIndex &index)
{
    if (addingFriend) return;
    QString selected = index.data().toString();
    if (selected.contains("(added) ")) selected.remove("(added) ");
    for (int i = 0; i < SOC.count(); i++)
    {
        if (SOC.at(i)->id == selected)
        {
            selectedId = SOC.at(i)->id;
            ui->post_text->setText(SOC.at(i)->post);
            ui->send_text->setText(SOC.at(i)->post);
            if (SOC.at(i)->added)
                ui->add_friend->setEnabled(false);
            else
                ui->add_friend->setEnabled(true);
        }
    }
}

void MainWindow::saveSettings()
{
    if (VKLOGIN == false) return;
    if (token.isEmpty()) return;
    if (auth->expires == NULL) return;
}

void MainWindow::on_add_friend_clicked()
{
    if (addingFriend) return;
    if (selectedId.isEmpty()) return;
    ui->add_friend->setEnabled(false);
    ui->add_friend->setText("Добавляю");
    disconnect(api, 0, this, 0);
    connect(api, SIGNAL(requestReady()), this, SLOT(handleAddFriend()));
    addingFriend = true;
    api->addFriend(selectedId, ui->send_text->toPlainText());
    selectedWhenCaptchaAppears = selectedId;
}

void MainWindow::handleAddFriend()
{
    if (api == NULL) return;
    if (api->xml_reply.contains("Captcha needed"))
    {
        CD = new CaptchaDialog();
        QString captchaUrl = "";
        QXmlStreamReader *xml2 = new QXmlStreamReader(api->xml_reply);
        while (!xml2->atEnd())
        {
            xml2->readNext();
            if (xml2->isStartElement())
            {
                if (xml2->name() == "captcha_img")
                {
                    captchaUrl = xml2->readElementText();
                }
                if (xml2->name() == "captcha_sid")
                {
                    captchaId = xml2->readElementText();
                }
            }
        }
        delete xml2;
        connect(CD, SIGNAL(ok()), this, SLOT(handleCaptcha()));
        CD->work(captchaUrl);
        CD->show();
        return;
    }
    if (api->xml_reply.contains("1"))
    {
        for (int i = 0; i < SOC.count(); i++)
        {
            if (SOC.at(i)->id == selectedId)
                SOC.at(i)->added = true;
        }
        updateList();
        addedCount++;
        updateInfoText();
    }
    addingFriend = false;
    ui->add_friend->setText("Добавить");
    selectedWhenCaptchaAppears = "";
}

void MainWindow::handleCaptcha()
{
    if (CD == NULL) return;
    if (CD->input.isEmpty()) return;
    captchaKey = CD->input;
    CD->deleteLater();
    api->addFriend(selectedId, ui->send_text->toPlainText(), captchaId, captchaKey);
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if (ui->checkBox_2->isChecked())
        ui->send_text->setReadOnly(false);
    else
        ui->send_text->setReadOnly(true);
}
