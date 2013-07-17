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

INCLUDEPATH += ../../src/ ../../submodules/ProjectTox-Core/core/
win32:INCLUDEPATH += ../../libs/sodium/include/

win32 {
    LIBS += -lWS2_32 ../../libs/sodium/lib/libsodium.a
} else {
    LIBS += -lsodium
}

win32:DEFINES += WIN32

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
    ../../src/core.cpp

HEADERS  += \
    ../../src/mainwindow.hpp \
    ../../submodules/ProjectTox-Core/core/DHT.h \
    ../../submodules/ProjectTox-Core/core/Lossless_UDP.h \
    ../../submodules/ProjectTox-Core/core/Messenger.h \
    ../../submodules/ProjectTox-Core/core/net_crypto.h \
    ../../submodules/ProjectTox-Core/core/network.h \
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

SOURCES += \
    ../../submodules/ProjectTox-Core/core/DHT.c \
    ../../submodules/ProjectTox-Core/core/Lossless_UDP.c \
    ../../submodules/ProjectTox-Core/core/Messenger.c \
    ../../submodules/ProjectTox-Core/core/net_crypto.c \
    ../../submodules/ProjectTox-Core/core/network.c

HEADERS  += \
    ../../submodules/ProjectTox-Core/core/DHT.h \
    ../../submodules/ProjectTox-Core/core/Lossless_UDP.h \
    ../../submodules/ProjectTox-Core/core/Messenger.h \
    ../../submodules/ProjectTox-Core/core/net_crypto.h \
    ../../submodules/ProjectTox-Core/core/network.h

RESOURCES += \
    ../../resources/icons.qrc
