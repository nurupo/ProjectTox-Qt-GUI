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

#include <QFile>
#include <QSettings>

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

    QString sFilename = FILENAME;

    //if no settings file exist -- use the default one
    QFile sFile(FILENAME);
    if (!sFile.exists()) {
        sFilename = ":/texts/" + FILENAME;
    }

    QSettings s(sFilename, QSettings::IniFormat);
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
        statusMessage = s.value("statusMessage", "My message").toString();
    s.endGroup();

    s.beginGroup("WindowSettings");
        QList<QString> windowNames = s.childGroups();
        for (const QString& name : windowNames) {
            s.beginGroup(name);
                windowSettings[name].geometry = s.value("geometry").toByteArray();
                windowSettings[name].state = s.value("state").toByteArray();
            s.endGroup();
        }
    s.endGroup();

    s.beginGroup("GUI");
        enableSmothAnimation = s.value("smothAnimation", true).toBool();
    s.endGroup();

    loaded = true;
}

void Settings::save()
{
    QSettings s(FILENAME, QSettings::IniFormat);

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

    s.beginGroup("WindowSettings");
    const QList<QString> windowNames = windowSettings.keys();
    for (const QString& name : windowNames) {
        s.beginGroup(name);
            s.setValue("geometry", windowSettings.value(name).geometry);
            s.setValue("state", windowSettings.value(name).state);
        s.endGroup();
    }
    s.endGroup();

    s.beginGroup("GUI");
        s.setValue("smothAnimation", enableSmothAnimation);
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

void Settings::saveWindow(const QMainWindow* window)
{
    windowSettings[window->objectName()].geometry = window->saveGeometry();
    windowSettings[window->objectName()].state = window->saveState();
}

void Settings::loadWindow(QMainWindow* window) const
{
    QHash<QString, WindowSettings>::const_iterator i = windowSettings.constFind(window->objectName());
    if (i == windowSettings.constEnd()) {
        return;
    }

    window->restoreGeometry(i.value().geometry);
    window->restoreState(i.value().state);
}

bool Settings::isAnimationEnabled() const
{
    return enableSmothAnimation;
}

void Settings::setAnimationEnabled(bool newValue)
{
    enableSmothAnimation = newValue;
}
