#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T14:56:46
#
#-------------------------------------------------

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    return(false)
}

!minQtVersion(5, 2, 0) {
    error("Cannot build with Qt version $${QT_VERSION}, this project requires at least Qt 5.2.0")
}

QT       += core gui widgets

TARGET = TOX-Qt-GUI
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ../../src/ ../../submodules/ProjectTox-Core/toxcore/
win32:INCLUDEPATH += ../../libs/sodium/include/
macx:INCLUDEPATH += /usr/local/include

win32 {
    LIBS += -lWS2_32 ../../libs/sodium/lib/libsodium.a
} else {
    macx {
        LIBS += -L/usr/local/lib -lsodium -lhunspell
    } else {
        LIBS += -lsodium -lhunspell
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
    ../../src/messagedisplaywidget.cpp \
    ../../src/opacitywidget.cpp \
    ../../src/customhintwidget.cpp \
    ../../src/Settings/guisettingspage.cpp \
    ../../src/Settings/emojifontcombobox.cpp \
    ../../src/smileypack.cpp \
    ../../src/Settings/emojifontsettingsdialog.cpp \
    ../../src/frienditemdelegate.cpp \
    ../../src/editablelabelwidget.cpp \
    ../../src/esclineedit.cpp \
    ../../src/copyableelidelabel.cpp \
    ../../src/spellchecker.cpp

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
    ../../src/messagedisplaywidget.hpp \
    ../../src/opacitywidget.hpp \
    ../../src/customhintwidget.hpp \
    ../../src/Settings/guisettingspage.hpp \
    ../../src/Settings/emojifontcombobox.hpp \
    ../../src/smileypack.hpp \
    ../../src/Settings/emojifontsettingsdialog.hpp \
    ../../src/frienditemdelegate.hpp \
    ../../src/editablelabelwidget.hpp \
    ../../src/esclineedit.hpp \
    ../../src/copyableelidelabel.hpp \
    ../../src/spellchecker.hpp

### ToxCore section. Please keep it alphabetical ###

SOURCES += \
    ../../submodules/ProjectTox-Core/toxcore/assoc.c \
    ../../submodules/ProjectTox-Core/toxcore/DHT.c \
    ../../submodules/ProjectTox-Core/toxcore/friend_requests.c \
    ../../submodules/ProjectTox-Core/toxcore/group_chats.c \
    ../../submodules/ProjectTox-Core/toxcore/LAN_discovery.c \
    ../../submodules/ProjectTox-Core/toxcore/Lossless_UDP.c \
    ../../submodules/ProjectTox-Core/toxcore/Messenger.c \
    ../../submodules/ProjectTox-Core/toxcore/net_crypto.c \
    ../../submodules/ProjectTox-Core/toxcore/network.c \
    ../../submodules/ProjectTox-Core/toxcore/onion.c \
    ../../submodules/ProjectTox-Core/toxcore/onion_announce.c \
    ../../submodules/ProjectTox-Core/toxcore/onion_client.c \
    ../../submodules/ProjectTox-Core/toxcore/ping.c \
    ../../submodules/ProjectTox-Core/toxcore/tox.c \
    ../../submodules/ProjectTox-Core/toxcore/util.c


HEADERS  += \
    ../../submodules/ProjectTox-Core/toxcore/assoc.h \
    ../../submodules/ProjectTox-Core/toxcore/DHT.h \
    ../../submodules/ProjectTox-Core/toxcore/friend_requests.h \
    ../../submodules/ProjectTox-Core/toxcore/group_chats.h \
    ../../submodules/ProjectTox-Core/toxcore/LAN_discovery.h \
    ../../submodules/ProjectTox-Core/toxcore/Lossless_UDP.h \
    ../../submodules/ProjectTox-Core/toxcore/Messenger.h \
    ../../submodules/ProjectTox-Core/toxcore/misc_tools.h \
    ../../submodules/ProjectTox-Core/toxcore/net_crypto.h \
    ../../submodules/ProjectTox-Core/toxcore/network.h \
    ../../submodules/ProjectTox-Core/toxcore/onion.h \
    ../../submodules/ProjectTox-Core/toxcore/onion_announce.h \
    ../../submodules/ProjectTox-Core/toxcore/onion_client.h \
    ../../submodules/ProjectTox-Core/toxcore/ping.h \
    ../../submodules/ProjectTox-Core/toxcore/tox.h \
    ../../submodules/ProjectTox-Core/toxcore/util.h


RESOURCES += \
    ../../resources/resources.qrc

win32:RC_FILE = ../../resources/win32_appicon.rc
