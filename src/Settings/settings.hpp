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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QMainWindow>

typedef char optKeyCode;

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings& getInstance();
    ~Settings();

    void load();
    void save();

    void executeSettingsDialog(QWidget* parent);

    static void saveWindow(const QMainWindow* window);
    static void loadWindow(QMainWindow* window);

    static const QString FILENAME;

    bool loaded;

    Settings();
    Settings(Settings &settings) = delete;
    Settings& operator=(const Settings&) = delete;

    struct DhtServer
    {
        QString name;
        QString userId;
        QString address;
        int port;
    };

    const QList<DhtServer>& getDhtServerList() const;
    void setDhtServerList(const QList<DhtServer>& newDhtServerList);

    QString getUsername() const;
    void setUsername(const QString& newUsername);

    bool getEnableLogging() const;
    void setEnableLogging(bool newValue);

    bool getEncryptLogs() const;
    void setEncryptLogs(bool newValue);
    QString getStatusMessage() const;
    void setStatusMessage(const QString& newMessage);

    QList<DhtServer> dhtServerList;
    int dhtServerId;
    bool dontShowDhtDialog;

    QString username;
    QString statusmessage;

    bool enableLogging;
    bool encryptLogs;

signals:
    //void dataChanged();
    void dhtServerListChanged();
    void logStorageOptsChanged();

};

#endif // SETTINGS_HPP
