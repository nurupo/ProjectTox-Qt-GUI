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

#include "core.hpp"

extern "C" {
#include <Messenger.h>
}

#ifndef WIN32
#include <arpa/inet.h>
#endif


#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <QTime>

Core* core;

Core::Core(const QString &dhtUserId, const QString &dhtIp, int dhtPort) :
    dhtUserId(dhtUserId), dhtIp(dhtIp), dhtPort(dhtPort)
{
    qRegisterMetaType<Core::FriendStatus>("FriendStatus");
    core = this;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Core::process);
}

Core::~Core()
{
}

void Core::friend_request(uint8_t * public_key, uint8_t * data, uint16_t length)
{
    qDebug() << "Friend request from" << uint8ToHex(public_key, 32) << "recieved";
    emit core->friendRequestRecieved(uint8ToHex(public_key, 32), uint8ToString(data, length));
}

void Core::friend_message(int friendnumber, uint8_t * message, uint16_t length)
{
    uint8_t user_id[32];
    if (getclient_id(friendnumber, user_id) == 0) {
        emit core->friendMessageRecieved(uint8ToHex(user_id, 32), uint8ToString(message, length));
    }
}

void Core::acceptFirendRequest(const QString &userId)
{
    // deleted when removed from userId QList
    uint8_t *user_id = new uint8_t[32];
    uint8FromHex(userId, user_id);
    
    m_addfriend_norequest(user_id);
    
    userIds << user_id;
}

void Core::requestFriendship(const QString& userId, const QString& message)
{
    // deleted when removed from userId QList
    uint8_t *user_id = new uint8_t[32];
    uint8FromHex(userId, user_id);

    uint8_t *msg = new uint8_t[message.length() * MAX_SIZE_OF_UTF8_ENCODED_CHARACTER];
    uint16_t msg_size = uint8FromString(message, msg);

    qDebug() << "checking message sizes" << msg_size << message.length();

    qDebug() << "requesting friendship of" << uint8ToHex(user_id, 32);
    qDebug() << "with message" << uint8ToString(msg, msg_size);

    qDebug() << "friend added with id" << m_addfriend(user_id, msg, msg_size);
    
    userIds << user_id;

    delete[] msg;
}

void Core::sendMessage(const QString &userId, const QString &message)
{
    uint8_t user_id[32];
    uint8FromHex(userId, user_id);
    int friend_id = getfriend_id(user_id);

    uint8_t *msg = new uint8_t[message.length() * MAX_SIZE_OF_UTF8_ENCODED_CHARACTER];
    uint16_t msg_size = uint8FromString(message, msg);

    m_sendmessage(friend_id, msg, msg_size);

    delete[] msg;
}

void Core::checkFriendsStatus()
{
    static QHash<uint8_t*, FriendStatus> old_status;
    for (uint8_t *user_id : userIds) {
        FriendStatus status = static_cast<FriendStatus>(m_friendstatus(getfriend_id(user_id)));
        if ((old_status.contains(user_id) && old_status[user_id] != status) || (!old_status.contains(user_id))) {
            QString userId = uint8ToHex(user_id, 32);
            emit friendStatusChanged(userId, status);
            old_status[user_id] = status;
        }
    }
}

void Core::removeFriend(const QString &userId)
{
    uint8_t user_id[32];
    uint8FromHex(userId, user_id);
    qDebug() << "removed friend ?" << (m_delfriend(getfriend_id(user_id)) == 0);
}

void Core::process()
{
    doMessenger();
    checkFriendsStatus();
    fflush(stdout);
}

void Core::run()
{
    initMessenger();
    m_callback_friendrequest(friend_request);
    m_callback_friendmessage(friend_message);

    qDebug() << "OUR ID:" << uint8ToHex(self_public_key, 32);
    emit userIdGererated(uint8ToHex(self_public_key, 32));

    perror("Initialization");
    IP_Port bootstrap_ip_port;
    bootstrap_ip_port.port = htons(dhtPort);
    bootstrap_ip_port.ip.i = inet_addr(dhtIp.toLatin1().data());
    uint8_t dht_user_id[32];
    uint8FromHex(dhtUserId, dht_user_id);
    DHT_bootstrap(bootstrap_ip_port, dht_user_id);

    timer->setInterval(30);
    timer->start();
}

QString Core::uint8ToString(uint8_t *data, uint16_t length)
{
    return QString::fromUtf8(reinterpret_cast<char*>(data), length);
}

uint16_t Core::uint8FromString(const QString &message, uint8_t *data)
{
    QByteArray arr = QByteArray(message.toUtf8());
    memcpy(data, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}

QString Core::uint8ToHex(uint8_t *data, uint16_t length)
{
    return QString(uint8ToByteArray(data, length).toHex()).toUpper();
}

uint16_t Core::uint8FromHex(const QString &hex, uint8_t *data)
{
    QByteArray arr = QByteArray::fromHex(hex.toLower().toLatin1());
    memcpy(data, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}

QByteArray Core::uint8ToByteArray(uint8_t *data, uint16_t length)
{
    return QByteArray(reinterpret_cast<char*>(data), length);
}
