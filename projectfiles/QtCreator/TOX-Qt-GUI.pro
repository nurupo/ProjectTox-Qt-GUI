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
macx:INCLUDEPATH += /usr/local/include

win32 {
    LIBS += -lWS2_32 ../../libs/sodium/lib/libsodium.a -ltoxcore.a
} else {
    macx {
        LIBS += -L/usr/local/lib -lsodium -ltoxcore
    } else {
        LIBS += -lsodium -ltoxcore
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
    ../../src/aboutdialog.cpp \
    ../../src/emoticonmenu.cpp \
    ../../src/messagelabel.cpp \
    ../../src/messagedisplaywidget.cpp

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
    ../../src/appinfo.hpp \
    ../../src/messagelabel.hpp \
    ../../src/emoticonmenu.hpp \
    ../../src/messagedisplaywidget.hpp

RESOURCES += \
    ../../resources/resources.qrc

win32:RC_FILE = ../../resources/win32_appicon.rc
