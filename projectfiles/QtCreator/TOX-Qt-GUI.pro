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

INCLUDEPATH += ../../src/ ../../deps/ProjectTox-Core/core
win32:INCLUDEPATH += ../../deps/sodium/include/

win32 {
    LIBS += -lWS2_32 ../../deps/sodium/lib/libsodium.a
} else {
    LIBS += -lsodium
}

SOURCES += \
    ../../src/main.cpp \
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
    ../../src/core.cpp \
    ../../deps/ProjectTox-Core/core/network.c \
    ../../deps/ProjectTox-Core/core/net_crypto.c \
    ../../deps/ProjectTox-Core/core/Messenger.c \
    ../../deps/ProjectTox-Core/core/Lossless_UDP.c \
    ../../deps/ProjectTox-Core/core/DHT.c

HEADERS  += \
    ../../src/mainwindow.hpp \
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
    ../../src/core.hpp \
    ../../deps/ProjectTox-Core/core/network.h \
    ../../deps/ProjectTox-Core/core/net_crypto.h \
    ../../deps/ProjectTox-Core/core/Messenger.h \
    ../../deps/ProjectTox-Core/core/Lossless_UDP.h \
    ../../deps/ProjectTox-Core/core/DHT.h

RESOURCES += \
    ../../resources/icons.qrc
