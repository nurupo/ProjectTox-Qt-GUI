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

INCLUDEPATH += ../../src/
win32:INCLUDEPATH += ../../src/sodium/include/

win32:LIBS += -lWS2_32 ../../src/sodium/lib/libsodium.a

SOURCES += \
    ../../src/main.cpp \
    ../../src/core/DHT.c \
    ../../src/core/Lossless_UDP.c \
    ../../src/core/Messenger.c \
    ../../src/core/net_crypto.c \
    ../../src/core/network.c \
    ../../src/mainwindow.cpp \
    ../../src/friendswidget.cpp \
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
    ../../src/renameeditwidget.cpp \
    ../../src/status.cpp \
    ../../src/friendrequestdialog.cpp \
    ../../src/customhinttextedit.cpp \
    ../../src/elidelabel.cpp \
    ../../src/dhtdialog.cpp \
    ../../src/core.cpp

HEADERS  += \
    ../../src/mainwindow.hpp \
    ../../src/core/DHT.h \
    ../../src/core/Lossless_UDP.h \
    ../../src/core/Messenger.h \
    ../../src/core/net_crypto.h \
    ../../src/core/network.h \
    ../../src/friendswidget.hpp \
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
    ../../src/renameeditwidget.hpp \
    ../../src/friendrequestdialog.hpp \
    ../../src/customhinttextedit.hpp \
    ../../src/elidelabel.hpp \
    ../../src/dhtdialog.hpp \
    ../../src/core.hpp

RESOURCES += \
    ../../resources/icons.qrc
