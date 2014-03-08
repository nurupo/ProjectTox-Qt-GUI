/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>
    
    This file is part of Tox Qt GUI.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    See the COPYING file for more details.
*/

#include "settings.hpp"
#include "settingsdialog.hpp"
#include "smileypack.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

const QString Settings::FILENAME = "settings.ini";

Settings::Settings() :
    loaded(false)
{
    load();
}

Settings::~Settings()
{
    save();
}

Settings& Settings::getInstance()
{
    static Settings settings;
    return settings;
}

void Settings::load()
{
    if (loaded) {
        return;
    }

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + FILENAME;

    //if no settings file exist -- use the default one
    QFile file(filePath);
    if (!file.exists()) {
        filePath = ":/texts/" + FILENAME;
    }

    QSettings s(filePath, QSettings::IniFormat);
    s.beginGroup("DHT Server");
        int serverListSize = s.beginReadArray("dhtServerList");
        for (int i = 0; i < serverListSize; i ++) {
            s.setArrayIndex(i);
            DhtServer server;
            server.name = s.value("name").toString();
            server.userId = s.value("userId").toString();
            server.address = s.value("address").toString();
            server.port = s.value("port").toInt();
            dhtServerList << server;
        }
        s.endArray();
    s.endGroup();

    //NOTE: uncomment when logging will be implemented
/*
    s.beginGroup("Logging");
       enableLogging = s.value("enableLogging", false).toBool();
       encryptLogs = s.value("encryptLogs", true).toBool();
    s.endGroup();
*/

    s.beginGroup("General");
        username = s.value("username", "My name").toString();
        statusMessage = s.value("statusMessage", "My status").toString();
    s.endGroup();

    s.beginGroup("Widgets");
        QList<QString> objectNames = s.childKeys();
        for (const QString& name : objectNames) {
            widgetSettings[name] = s.value(name).toByteArray();
        }
    s.endGroup();

    s.beginGroup("GUI");
        enableSmoothAnimation = s.value("smoothAnimation", true).toBool();
        smileyPack = s.value("smileyPack").toByteArray();
        customEmojiFont = s.value("customEmojiFont", true).toBool();
        emojiFontFamily = s.value("emojiFontFamily", "DejaVu Sans").toString();
        emojiFontPointSize = s.value("emojiFontPointSize", QApplication::font().pointSize()).toInt();
    s.endGroup();

    s.beginGroup("Sound");
        soundEnabled = s.value("enabled", true).toBool();
        soundPackDirName = s.value("soundPackDirName", "tox").toString();
        playOnError = s.value("playOnError", true).toBool();
        playOnLogInOut = s.value("playOnLogInOut", true).toBool();
        playOnFriendLogsInOut = s.value("playOnFriendLogsInOut", true).toBool();
        playOnFriendRequestAccepted = s.value("playOnFriendRequestAccepted", true).toBool();
        playOnIncommingCall = s.value("playOnIncommingCall", true).toBool();
        playOnOutgoingCall = s.value("playOnOutgoingCall", true).toBool();
        playOnNewMessage = s.value("playOnNewMessage", true).toBool();
        playOnFiletransferComplete = s.value("playOnFiletransferComplete", true).toBool();
        playOnFiletransferPending = s.value("playOnFiletransferPending", true).toBool();
    s.endGroup();

    loaded = true;
}
bool Settings::isPlayOnFiletransferPending() const
{
    return playOnFiletransferPending;
}

void Settings::setPlayOnFiletransferPending(bool value)
{
    playOnFiletransferPending = value;
}

bool Settings::isPlayOn(const Sound &sound)
{
    switch (sound) {
    case Sound::Error:                return playOnError;
    case Sound::FriendLogsIn:
    case Sound::FriendLogsOut:        return playOnFriendLogsInOut;
    case Sound::FriendRequestAccepted:return playOnFriendRequestAccepted;
    case Sound::IncommingCall:
    case Sound::IncommingVideoCall:   return playOnIncommingCall;
    case Sound::LogIn:
    case Sound::LogOut:               return playOnLogInOut;
    case Sound::NewMessage:           return playOnNewMessage;
    case Sound::OutgoingCall:
    case Sound::OutgoingVideoCall:    return playOnOutgoingCall;
    case Sound::FiletransferComplete: return playOnFiletransferComplete;
    case Sound::FiletransferPending:  return playOnFiletransferPending;
    }
    return false;
}

bool Settings::isPlayOnFiletransferComplete() const
{
    return playOnFiletransferComplete;
}

void Settings::setPlayOnFiletransferComplete(bool value)
{
    playOnFiletransferComplete = value;
}

bool Settings::isPlayOnNewMessage() const
{
    return playOnNewMessage;
}

void Settings::setPlayOnNewMessage(bool value)
{
    playOnNewMessage = value;
}

bool Settings::isPlayOnOutgoingCall() const
{
    return playOnOutgoingCall;
}

void Settings::setPlayOnOutgoingCall(bool value)
{
    playOnOutgoingCall = value;
}

bool Settings::isPlayOnIncommingCall() const
{
    return playOnIncommingCall;
}

void Settings::setPlayOnIncommingCall(bool value)
{
    playOnIncommingCall = value;
}

bool Settings::isPlayOnFriendRequestAccepted() const
{
    return playOnFriendRequestAccepted;
}

void Settings::setPlayOnFriendRequestAccepted(bool value)
{
    playOnFriendRequestAccepted = value;
}

bool Settings::isPlayOnFriendLogsInOut() const
{
    return playOnFriendLogsInOut;
}

void Settings::setPlayOnFriendLogsInOut(bool value)
{
    playOnFriendLogsInOut = value;
}

bool Settings::isPlayOnLogInOut() const
{
    return playOnLogInOut;
}

void Settings::setPlayOnLogInOut(bool value)
{
    playOnLogInOut = value;
}


void Settings::save()
{
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + FILENAME;

    QSettings s(filePath, QSettings::IniFormat);

    s.clear();

    s.beginGroup("DHT Server");
        s.beginWriteArray("dhtServerList", dhtServerList.size());
        for (int i = 0; i < dhtServerList.size(); i ++) {
            s.setArrayIndex(i);
            s.setValue("name", dhtServerList[i].name);
            s.setValue("userId", dhtServerList[i].userId);
            s.setValue("address", dhtServerList[i].address);
            s.setValue("port", dhtServerList[i].port);
        }
        s.endArray();
    s.endGroup();

    //NOTE: uncomment when logging will be implemented
/*
    s.beginGroup("Logging");
        s.setValue("storeLogs", enableLogging);
        s.setValue("encryptLogs", encryptLogs);
    s.endGroup();
*/

    s.beginGroup("General");
        s.setValue("username", username);
        s.setValue("statusMessage", statusMessage);
    s.endGroup();

    s.beginGroup("Widgets");
    const QList<QString> widgetNames = widgetSettings.keys();
    for (const QString& name : widgetNames) {
        s.setValue(name, widgetSettings.value(name));
    }
    s.endGroup();

    s.beginGroup("GUI");
        s.setValue("smoothAnimation", enableSmoothAnimation);
        s.setValue("smileyPack", smileyPack);
        s.setValue("customEmojiFont", customEmojiFont);
        s.setValue("emojiFontFamily", emojiFontFamily);
        s.setValue("emojiFontPointSize", emojiFontPointSize);
    s.endGroup();

    s.beginGroup("Sound");
        s.setValue("enabled", soundEnabled);
        s.setValue("soundPackDirName", soundPackDirName);
        s.setValue("playOnError", playOnError);
        s.setValue("playOnLogInOut", playOnLogInOut);
        s.setValue("playOnFriendLogsInOut", playOnFriendLogsInOut);
        s.setValue("playOnFriendRequestAccepted", playOnFriendRequestAccepted);
        s.setValue("playOnIncommingCall", playOnIncommingCall);
        s.setValue("playOnOutgoingCall", playOnOutgoingCall);
        s.setValue("playOnNewMessage", playOnNewMessage);
        s.setValue("playOnFiletransferComplete", playOnFiletransferComplete);
        s.setValue("playOnFiletransferPending", playOnFiletransferPending);
    s.endGroup();
}

void Settings::executeSettingsDialog(QWidget* parent)
{
    SettingsDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) {
        save();
        //emit dataChanged();
    }
}

const QList<Settings::DhtServer>& Settings::getDhtServerList() const
{
    return dhtServerList;
}

void Settings::setDhtServerList(const QList<DhtServer>& newDhtServerList)
{
    dhtServerList = newDhtServerList;
    emit dhtServerListChanged();
}

QString Settings::getUsername() const
{
    return username;
}

void Settings::setUsername(const QString& newUsername)
{
    username = newUsername;
}

QString Settings::getStatusMessage() const
{
    return statusMessage;
}

void Settings::setStatusMessage(const QString& newMessage)
{
    statusMessage = newMessage;
}

bool Settings::getEnableLogging() const
{
    return enableLogging;
}

void Settings::setEnableLogging(bool newValue)
{
    enableLogging = newValue;
}

bool Settings::getEncryptLogs() const
{
    return encryptLogs;
}

void Settings::setEncryptLogs(bool newValue)
{
    encryptLogs = newValue;
}

void Settings::setWidgetData(const QString& uniqueName, const QByteArray& data)
{
    widgetSettings[uniqueName] = data;
}

QByteArray Settings::getWidgetData(const QString& uniqueName) const
{
    return widgetSettings.value(uniqueName);
}

bool Settings::isAnimationEnabled() const
{
    return enableSmoothAnimation;
}

void Settings::setAnimationEnabled(bool newValue)
{
    enableSmoothAnimation = newValue;
}

QByteArray Settings::getSmileyPack() const
{
    return smileyPack;
}

void Settings::setSmileyPack(const QByteArray &value)
{
    smileyPack = value;
    emit smileyPackChanged();
}

bool Settings::isCurstomEmojiFont() const
{
    return customEmojiFont;
}

void Settings::setCurstomEmojiFont(bool value)
{
    customEmojiFont = value;
    emit emojiFontChanged();
}

int Settings::getEmojiFontPointSize() const
{
    return emojiFontPointSize;
}

void Settings::setEmojiFontPointSize(int value)
{
    emojiFontPointSize = value;
    emit emojiFontChanged();
}

bool Settings::isSoundEnabled() const
{
    return soundEnabled;
}

void Settings::setSoundEnabled(bool value)
{
    soundEnabled = value;
}

QString Settings::getsoundPackDirName() const
{
    return soundPackDirName;
}

void Settings::setsoundPackDirName(const QString &value)
{
    soundPackDirName = value;
    emit soundPackChanged(value);
}

bool Settings::isPlayOnError() const
{
    return playOnError;
}

void Settings::setPlayOnError(bool value)
{
    playOnError = value;
}

QString Settings::getEmojiFontFamily() const
{
    return emojiFontFamily;
}

void Settings::setEmojiFontFamily(const QString &value)
{
    emojiFontFamily = value;
    emit emojiFontChanged();
}
