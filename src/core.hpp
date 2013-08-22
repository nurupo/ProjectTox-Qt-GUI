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

#include <QObject>
#include <QTimer>
#include <QList>

class Core : public QObject
{
    Q_OBJECT
    Q_ENUMS(FriendStatus)

public:
    explicit Core();

    enum class FriendStatus {NotFound = 0, Added, RequestSent, Confirmed, Online};

private:
    static void onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize);
    static void onFriendMessage(int friendId, uint8_t* cMessage, uint16_t cMessageSize);
    static void onFriendNameChange(int friendId, uint8_t* cName, uint16_t cNameSize);
    static void onStatusMessageChanged(int friendId, uint8_t* cMessage, uint16_t cMessageSize);
    static void onFriendStatusChanged(int friendId, uint8_t status);

    void checkConnection();

    QTimer* timer;
    QList<int> friendIdList;

    class CUserId
    {
    public:
        explicit CUserId(const QString& userId);
        ~CUserId();

        uint8_t* data();
        uint16_t size();

        static QString toString(uint8_t* cUserId/*, uint16_t cUserIdSize*/);

    private:
        uint8_t* cUserId;
        uint16_t cUserIdSize;

        static uint16_t fromString(const QString& userId, uint8_t* cUserId);
    };

    class CString
    {
    public:
        explicit CString(const QString& string);
        ~CString();

        uint8_t* data();
        uint16_t size();

        static QString toString(uint8_t* cMessage, uint16_t cMessageSize);
        static QString toString(uint8_t* cMessage);


    private:
        const static int MAX_SIZE_OF_UTF8_ENCODED_CHARACTER = 4;

        uint8_t* cString;
        uint16_t cStringSize;

        static uint16_t fromString(const QString& message, uint8_t* cMessage);
    };

public slots:
    void start();

    void acceptFriendRequest(const QString& userId);
    void requestFriendship(const QString& userId, const QString& message);

    void removeFriend(int friendId);

    void sendMessage(int friendId, const QString& message);

    void setUsername(const QString& username);
    void setStatusMessage(const QString& message);

    void process();

    void bootstrapDht();

signals:
    void connected();
    void disconnected();

    void friendRequestRecieved(const QString& userId, const QString& message);
    void friendMessageRecieved(int friendId, const QString& message);

    void friendAdded(int friendId, const QString& userId);

    void friendStatusChanged(int friendId, FriendStatus status);
    void friendStatusMessageChanged(int friendId, const QString& message);
    void friendUsernameChanged(int friendId, const QString& username);

    void userIdGenerated(const QString& userId);

    void friendRemoved(int friendId);

    void usernameSet(const QString& username);
    void statusMessageSet(const QString& message);

    void messageSentResult(int friendId, const QString& message, int messageId);

    void failedToAddFriend(const QString& userId);
    void failedToRemoveFriend(int friendId);
    void failedToSetUsername(const QString& username);
    void failedToSetStatusMessage(const QString& message);

};

Q_DECLARE_METATYPE(Core::FriendStatus)

#endif // CORE_HPP

