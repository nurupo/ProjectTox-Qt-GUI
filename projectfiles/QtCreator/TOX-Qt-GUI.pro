#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T14:56:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TOX-Qt-GUI
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ../../src/src/

SOURCES += \
    ../../src/main.cpp \
#   core/DHT.c \
    ../../src/mainwindow.cpp \
#   settingsdialog.cpp \
#   dhtworker.cpp \
    ../../src/friendswidget.cpp \
    ../../src/frienditemdelegate.cpp \
    ../../src/addfrienddialog.cpp \
    ../../src/friendproxymodel.cpp \
    ../../src/filterwidget.cpp \
    ../../src/customhinttreeview.cpp \
    ../../src/chatpagewidget.cpp \
    ../../src/pageswidget.cpp \
    ../../src/inputtextwidget.cpp \
    ../../src/messagedisplaywidget.cpp \
    ../../src/frienditemwidget.cpp \
    ../../src/ouruseritemwidget.cpp \
    ../../src/imagecombobox.cpp \
    ../../src/renameeditwidget.cpp \
    ../../src/status.cpp

HEADERS  += \
    ../../src/mainwindow.hpp \
#   core/DHT.h \
#   settingsdialog.hpp \
#   dhtworker.hpp \
    ../../src/friendswidget.hpp \
    ../../src/frienditemdelegate.hpp \
    ../../src/addfrienddialog.hpp \
    ../../src/friendproxymodel.hpp \
    ../../src/status.hpp \
    ../../src/filterwidget.hpp \
    ../../src/customhinttreeview.hpp \
    ../../src/chatpagewidget.hpp \
    ../../src/pageswidget.hpp \
    ../../src/inputtextwidget.hpp \
    ../../src/messagedisplaywidget.hpp \
    ../../src/frienditemwidget.hpp \
    ../../src/ouruseritemwidget.hpp \
    ../../src/imagecombobox.hpp \
    ../../src/renameeditwidget.hpp


win32:LIBS += -lWS2_32

RESOURCES += \
    ../../resources/icons.qrc
