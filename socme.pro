#-------------------------------------------------
#
# Project created by QtCreator 2014-03-08T19:15:19
#
#-------------------------------------------------

QT       += webkitwidgets core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = autosoc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vk_auth.cpp \
    webwidget.cpp \
    vk_api.cpp \
    socgetter.cpp \
    captchadialog.cpp

HEADERS  += mainwindow.h \
    vk_auth.h \
    webwidget.h \
    vk_api.h \
    socgetter.h \
    captchadialog.h

FORMS    += mainwindow.ui \
    webwidget.ui \
    captchadialog.ui
