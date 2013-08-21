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

win32 {
    INCLUDEPATH += ../../libs/sodium/include/
    LIBS += -lWS2_32 ../../libs/sodium/lib/libsodium.a
} else {
    macx-g++ {
        # To build a Mac OS X 10.6 compatible executable,
        # we use MacPorts gcc 4.7 (to install: sudo port install gcc47)

        # Why? Because the default gcc on Mac OS X (currently 4.2)
        # doesn't support c++11

        # Define a custom Qt5 Desktop kit using gcc instead of clang
        # in QtCreator and select this kit for compilation

        QMAKE_CXX = /opt/local/bin/g++-mp-4.7

        INCLUDEPATH += /usr/local/include
        LIBS += -L/usr/local/lib -lsodium
    } else {
        macx {
            # To build a Mac OS X 10.7 (and higher) compatible executable,
            # clang can be used directly

            # for proper c++11 support we have to add
            QMAKE_CXXFLAGS += -stdlib=libc++
            # since -stdlib=libc++ requires 10.7 we have to add also
            QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
            QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

            INCLUDEPATH += /usr/local/include
            LIBS += -L/usr/local/lib -lsodium
        } else {
            LIBS += -lsodium
        }
    }
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
    ../../src/core.cpp \
    ../../src/Settings/abstractsettingspage.cpp \
    ../../src/Settings/basicsettingsdialog.cpp \
    ../../src/Settings/settings.cpp \
    ../../src/closeapplicationdialog.cpp \
    ../../src/starter.cpp \
    ../../src/Settings/settingsdialog.cpp \
    ../../src/Settings/dhtbootstrapsettingspage.cpp \
    ../../src/Settings/dhtserverdialog.cpp \
    ../../src/Settings/customhintlistwidget.cpp \
    ../../src/Settings/loggingsettingspage.cpp \
    ../../src/aboutdialog.cpp

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
    ../../src/core.hpp \
    ../../src/Settings/abstractsettingspage.hpp \
    ../../src/Settings/basicsettingsdialog.hpp \
    ../../src/Settings/settings.hpp \
    ../../src/closeapplicationdialog.hpp \
    ../../src/starter.hpp \
    ../../src/Settings/settingsdialog.hpp \
    ../../src/Settings/dhtbootstrapsettingspage.hpp \
    ../../src/Settings/dhtserverdialog.hpp \
    ../../src/Settings/customhintlistwidget.hpp \
    ../../src/Settings/loggingsettingspage.hpp \
    ../../src/aboutdialog.hpp \
    ../../src/appinfo.hpp

SOURCES += \
    ../../submodules/ProjectTox-Core/core/DHT.c \
    ../../submodules/ProjectTox-Core/core/Lossless_UDP.c \
    ../../submodules/ProjectTox-Core/core/Messenger.c \
    ../../submodules/ProjectTox-Core/core/net_crypto.c \
    ../../submodules/ProjectTox-Core/core/network.c \
    ../../submodules/ProjectTox-Core/core/friend_requests.c \
    ../../submodules/ProjectTox-Core/core/LAN_discovery.c \
    ../../submodules/ProjectTox-Core/core/ping.c \
    ../../submodules/ProjectTox-Core/core/util.c

HEADERS  += \
    ../../submodules/ProjectTox-Core/core/DHT.h \
    ../../submodules/ProjectTox-Core/core/Lossless_UDP.h \
    ../../submodules/ProjectTox-Core/core/Messenger.h \
    ../../submodules/ProjectTox-Core/core/net_crypto.h \
    ../../submodules/ProjectTox-Core/core/network.h \
    ../../submodules/ProjectTox-Core/core/friend_requests.h \
    ../../submodules/ProjectTox-Core/core/LAN_discovery.h \
    ../../submodules/ProjectTox-Core/core/ping.h \
    ../../submodules/ProjectTox-Core/core/util.h

RESOURCES += \
    ../../resources/resources.qrc

win32:RC_FILE = ../../resources/win32_appicon.rc
