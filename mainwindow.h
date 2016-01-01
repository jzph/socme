#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
namespace Ui {
class MainWindow;
}
class VK_auth;
class VK_api;
class SocFriendsLoader;
class QPixmap;
class CaptchaDialog;
struct SocFriend
{
    bool added;
    bool inFriends;
    QString post;
    QString id;
    QString message;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    VK_auth* auth;
    VK_api* api;
    SocFriendsLoader* SFL;
    bool VKLOGIN;
    bool addingFriend;
    bool gettingSoc;
    QString token;
    QString user_id;
    QString first_name;
    QString last_name;
    QString selectedId;
    QString selectedWhenCaptchaAppears;
    QPixmap* photo;
    QList<QString> friends;
    QList<QString> reqFriends;
    QList<QString> thread_id;
    QList<SocFriend*> SOC;
    int alreadyInFriends;
    int addedCount;
    CaptchaDialog *CD;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString captchaId;
    QString captchaKey;
private slots:
    void on_vk_login_clicked();
    void handleLogin();
    void handleLoginFailed();
    void handleApiUser();
    void handleApiUserPhoto();
    void handleFriends();
    void handleRequestedFriends();
    void handleSFL();
    void on_soc_thread_clicked();
    void handleList(const QModelIndex &index);
    void handleAddFriend();
    void handleCaptcha();

    void on_add_friend_clicked();

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    void saveSettings();
    void loadSettings();
    void updateList();
    void updateInfoText();
};

#endif // MAINWINDOW_H
