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
#include <QHash>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(const QString &dhtUserId, const QString &dhtIp, int dhtPort);
    ~Core();
    void run();

    enum class FriendStatus {NotFound = 0, Added, RequestSent, Confirmed, Online};

private:
    static QString uint8ToString(uint8_t *data, uint16_t length);
    static uint16_t uint8FromString(const QString &message, uint8_t *data);

    static QString uint8ToHex(uint8_t *data, uint16_t length);
    static uint16_t uint8FromHex(const QString &hex, uint8_t *data);

    static QByteArray uint8ToByteArray(uint8_t *data, uint16_t length);

    static void friend_request(uint8_t * public_key, uint8_t * data, uint16_t length);
    static void friend_message(int friendnumber, uint8_t * message, uint16_t length);

    void checkFriendsStatus();

    const static int MAX_SIZE_OF_UTF8_ENCODED_CHARACTER = 4;

    QString dhtUserId;
    QString dhtIp;
    int dhtPort;

    QTimer* timer;
    QList<uint8_t*> userIds;

public slots:
    void sendMessage(const QString &userId, const QString &message);
    void acceptFirendRequest(const QString &userId);
    void requestFriendship(const QString& userIdChanged, const QString& message);
    void removeFriend(const QString &userId);
    void process();

signals:
    void friendRequestRecieved(const QString& userId, const QString& message);
    void friendMessageRecieved(const QString& userId, const QString& message);
    void userIdGererated(const QString& userId);
    void friendStatusChanged(const QString& userId, FriendStatus status);

};

Q_DECLARE_METATYPE(Core::FriendStatus)

#endif // CORE_HPP

