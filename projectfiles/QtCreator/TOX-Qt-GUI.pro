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

INCLUDEPATH += ../../src/
win32:INCLUDEPATH += ../../libs/include/
macx:INCLUDEPATH += /usr/local/include/

win32 {
    LIBS += ../../libs/lib/libtoxcore.a -lws2_32 ../../libs/lib/libsodium.a ../../libs/lib/libhunspell.a
} else {
    macx {
        LIBS += -L/usr/local/lib -ltoxcore -lsodium -lhunspell
    } else {
        LIBS += -ltoxcore -lsodium -lhunspell
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
    ../../src/messages/messagemodel.cpp \
    ../../src/messages/message.cpp \
    ../../src/messages/messagemodelitem.cpp \
    ../../src/messages/chatscene.cpp \
    ../../src/messages/chatitem.cpp \
    ../../src/messages/chatline.cpp \
    ../../src/messages/chatview.cpp \
    ../../src/messages/clickable.cpp \
    ../../src/messages/markerlineitem.cpp \
    ../../src/messages/columnhandleitem.cpp \
    ../../src/messages/smileytextobject.cpp \
    ../../src/messages/smiley.cpp \
    ../../src/messages/messagefilter.cpp \
    ../../src/messages/chatviewsearchwidget.cpp \
    ../../src/Settings/privacysettingspage.cpp \
    ../../src/messages/typingitem.cpp

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
    ../../src/emoticonmenu.hpp \
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
    ../../src/messages/id.hpp \
    ../../src/messages/messagemodel.hpp \
    ../../src/messages/message.hpp \
    ../../src/messages/messagemodelitem.hpp \
    ../../src/messages/chatscene.hpp \
    ../../src/messages/chatitem.hpp \
    ../../src/messages/chatline.hpp \
    ../../src/messages/chatview.hpp \
    ../../src/messages/clickable.hpp \
    ../../src/messages/markerlineitem.hpp \
    ../../src/messages/columnhandleitem.hpp \
    ../../src/messages/smileytextobject.hpp \
    ../../src/messages/smiley.hpp \
    ../../src/messages/messagefilter.hpp \
    ../../src/messages/chatviewsearchwidget.hpp \
    ../../src/Settings/privacysettingspage.hpp \
    ../../src/messages/typingitem.hpp

RESOURCES += \
    ../../resources/resources.qrc

win32:RC_FILE = ../../resources/win32_appicon.rc
