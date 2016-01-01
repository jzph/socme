#ifndef WEBWIDGET_H
#define WEBWIDGET_H

#include <QMainWindow>

namespace Ui {
class webWidget;
}
class QWebView;

class webWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit webWidget(QWidget *parent = 0);
    ~webWidget();
    QWebView* getWebView();
private:
    Ui::webWidget *ui;
};

#endif // WEBWIDGET_H
