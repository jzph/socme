#include "webwidget.h"
#include "ui_webwidget.h"

webWidget::webWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webWidget)
{
    ui->setupUi(this);
}

webWidget::~webWidget()
{
    delete ui;
}

QWebView* webWidget::getWebView()
{
    return ui->webView;
}
