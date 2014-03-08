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

#include <QHash>
#include <QMainWindow>
#include <QSplitter>
#include "soundmanager.hpp"

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings& getInstance();
    ~Settings();

    void executeSettingsDialog(QWidget* parent);

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

    QString getStatusMessage() const;
    void setStatusMessage(const QString& newMessage);

    bool getEnableLogging() const;
    void setEnableLogging(bool newValue);

    bool getEncryptLogs() const;
    void setEncryptLogs(bool newValue);

    // Assume all widgets have unique names
    // Don't use it to save every single thing you want to save, use it
    // for some general purpose widgets, such as MainWindows or Splitters,
    // which have widget->saveX() and widget->loadX() methods.
    QByteArray getWidgetData(const QString& uniqueName) const;
    void setWidgetData(const QString& uniqueName, const QByteArray& data);

    // Wrappers around getWidgetData() and setWidgetData()
    // Assume widget has a unique objectName set
    template <class T>
    void restoreGeometryState(T* widget) const
    {
        widget->restoreGeometry(getWidgetData(widget->objectName() + "Geometry"));
        widget->restoreState(getWidgetData(widget->objectName() + "State"));
    }
    template <class T>
    void saveGeometryState(const T* widget)
    {
        setWidgetData(widget->objectName() + "Geometry", widget->saveGeometry());
        setWidgetData(widget->objectName() + "State", widget->saveState());
    }

    bool isAnimationEnabled() const;
    void setAnimationEnabled(bool newValue);

    QByteArray getSmileyPack() const;
    void setSmileyPack(const QByteArray &value);

    bool isCurstomEmojiFont() const;
    void setCurstomEmojiFont(bool value);

    QString getEmojiFontFamily() const;
    void setEmojiFontFamily(const QString &value);

    int getEmojiFontPointSize() const;
    void setEmojiFontPointSize(int value);

    bool isSoundEnabled() const;
    void setSoundEnabled(bool value);

    QString getsoundPackDirName() const;
    void setsoundPackDirName(const QString &value);

    bool isPlayOnError() const;
    void setPlayOnError(bool value);

    bool isPlayOnLogInOut() const;
    void setPlayOnLogInOut(bool value);

    bool isPlayOnFriendLogsInOut() const;
    void setPlayOnFriendLogsInOut(bool value);

    bool isPlayOnFriendRequestAccepted() const;
    void setPlayOnFriendRequestAccepted(bool value);

    bool isPlayOnIncommingCall() const;
    void setPlayOnIncommingCall(bool value);

    bool isPlayOnOutgoingCall() const;
    void setPlayOnOutgoingCall(bool value);

    bool isPlayOnNewMessage() const;
    void setPlayOnNewMessage(bool value);

    bool isPlayOnFiletransferComplete() const;
    void setPlayOnFiletransferComplete(bool value);

    bool isPlayOnFiletransferPending() const;
    void setPlayOnFiletransferPending(bool value);

    bool isPlayOn(const Sound &sound);

private:
    Settings();
    Settings(Settings &settings) = delete;
    Settings& operator=(const Settings&) = delete;

    void save();
    void load();

    static const QString FILENAME;

    bool loaded;

    QList<DhtServer> dhtServerList;
    int dhtServerId;
    bool dontShowDhtDialog;

    QString username;
    QString statusMessage;

    bool enableLogging;
    bool encryptLogs;

    QHash<QString, QByteArray> widgetSettings;

    // GUI
    bool enableSmoothAnimation;
    QByteArray smileyPack;
    bool customEmojiFont;
    QString emojiFontFamily;
    int     emojiFontPointSize;

    // Sound
    bool    soundEnabled;
    QString soundPackDirName;
    bool playOnError;
    bool playOnLogInOut;
    bool playOnFriendLogsInOut;
    bool playOnFriendRequestAccepted;
    bool playOnIncommingCall;
    bool playOnOutgoingCall;
    bool playOnNewMessage;
    bool playOnFiletransferComplete;
    bool playOnFiletransferPending;

signals:
    //void dataChanged();
    void dhtServerListChanged();
    void logStorageOptsChanged();
    void smileyPackChanged();
    void emojiFontChanged();
    void soundPackChanged(const QString &pack);
};

#endif // SETTINGS_HPP
