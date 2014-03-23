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

#ifndef CORE_HPP
#define CORE_HPP

#include "status.hpp"

#include <tox.h>

#include <QObject>
#include <QThread>
#include <QTimer>

#ifdef QT_GUI_TOX_WAIT
class WaitWrapper;
#endif

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

private:
    static void onFriendRequest(Tox* tox, uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendMessage(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendNameChange(Tox* tox, int friendId, uint8_t* cName, uint16_t cNameSize, void* core);
    static void onStatusMessageChanged(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onUserStatusChanged(Tox* tox, int friendId, uint8_t userstatus, void* core);
    static void onConnectionStatusChanged(Tox* tox, int friendId, uint8_t status, void* core);
    static void onAction(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);

    void checkConnection();

    Tox* tox;

#ifdef QT_GUI_TOX_WAIT
    WaitWrapper* waitWrapper;
#else
    QTimer* timer;
#endif

    class CData
    {
    public:
        uint8_t* data();
        uint16_t size();

    protected:
        explicit CData(const QString& data, uint16_t byteSize);
        virtual ~CData();

        static QString toString(uint8_t* cData, uint16_t cDataSize);

    private:
        uint8_t* cData;
        uint16_t cDataSize;

        static uint16_t fromString(const QString& userId, uint8_t* cData);
    };

    class CUserId : public CData
    {
    public:
        explicit CUserId(const QString& userId);

        static QString toString(uint8_t* cUserId);

    private:
        static const uint16_t SIZE = TOX_CLIENT_ID_SIZE;

    };

    class CFriendAddress : public CData
    {
    public:
        explicit CFriendAddress(const QString& friendAddress);

        static QString toString(uint8_t* cFriendAddress);

    private:
        static const uint16_t SIZE = TOX_FRIEND_ADDRESS_SIZE;

    };

    class CString
    {
    public:
        explicit CString(const QString& string);
        ~CString();

        uint8_t* data();
        uint16_t size();

        static QString toString(uint8_t* cMessage, uint16_t cMessageSize);

    private:
        const static int MAX_SIZE_OF_UTF8_ENCODED_CHARACTER = 4;

        uint8_t* cString;
        uint16_t cStringSize;

        static uint16_t fromString(const QString& message, uint8_t* cMessage);
    };

public slots:
    void start();

    void acceptFriendRequest(const QString& userId);
    void requestFriendship(const QString& friendAddress, const QString& message);

    void removeFriend(int friendId);

    void sendMessage(int friendId, const QString& message);
    void sendAction(int friendId, const QString& action);

    void setUsername(const QString& username);
    void setStatusMessage(const QString& message);
    void setStatus(Status status);

    void process();

    void bootstrapDht();

signals:
    void connected();
    void disconnected();

    void friendRequestRecieved(const QString& userId, const QString& message);
    void friendMessageRecieved(int friendId, const QString& message);

    void friendAdded(int friendId, const QString& userId);

    void friendStatusChanged(int friendId, Status status);
    void friendStatusMessageChanged(int friendId, const QString& message);
    void friendUsernameChanged(int friendId, const QString& username);

    void friendAddressGenerated(const QString& friendAddress);

    void friendRemoved(int friendId);

    void usernameSet(const QString& username);
    void statusMessageSet(const QString& message);
    void statusSet(Status status);

    void messageSentResult(int friendId, const QString& message, int messageId);
    void actionSentResult(int friendId, const QString& action, int success);

    void failedToAddFriend(const QString& userId);
    void failedToRemoveFriend(int friendId);
    void failedToSetUsername(const QString& username);
    void failedToSetStatusMessage(const QString& message);
    void failedToSetStatus(Status status);

    void actionReceived(int friendId, const QString& acionMessage);

    void failedToStart();

};

#ifdef QT_GUI_TOX_WAIT
class Wait : public QObject
{
    Q_OBJECT
public:
    Wait(QObject* parent = 0);
public slots:
    void run(uint8_t* data, long seconds, long microseconds);

signals:
    void wokeUp();
};

class WaitWrapper : public QObject
{
    Q_OBJECT
public:
    WaitWrapper(Tox *tox, QObject* parent = 0);
    ~WaitWrapper();

    long getWaitTimeSeconds();
    long getWaitTimeMicroseconds();

public slots:
    void setWaitTimeSeconds(long seconds);
    void setWaitTimeMicroseconds(long microseconds);
    void setWaitTime(long seconds, long microseconds);
    void run();

private slots:
    void onWakeUp();
signals:
    void wokeUp();

private:

    long seconds;
    long microseconds;
    uint8_t* data;
    QThread* thread;
    Wait wait;
    Tox* tox;
};
#endif

#endif // CORE_HPP

