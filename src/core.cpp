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
#include "Settings/settings.hpp"

#include <Messenger.h>

#ifndef WIN32
#include <arpa/inet.h>
#endif

#include <QThread>
#include <QTime>
#include <QFile>

#include <QDebug>

#define MESSENGER_BAK "save.bak"

Core::Core() :
    QObject(nullptr)
{
    qRegisterMetaType<Core::FriendStatus>("FriendStatus");
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Core::process);
    connect(&Settings::getInstance(), &Settings::dhtServerListChanged, this, &Core::bootstrapDht);
}

Core::~Core()
{
    uint32_t size = Messenger_size(messenger);
    QByteArray buf(size, 0);
    Messenger_save(messenger, reinterpret_cast<uint8_t*>(buf.data()));
    QFile file(MESSENGER_BAK);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to write save.bak";
	return;
    }
    file.write(buf);
    file.close();
}

void Core::onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendRequestRecieved(CUserId::toString(cUserId), CString::toString(cMessage, cMessageSize));
}

void Core::onFriendMessage(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendMessageRecieved(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onFriendNameChange(Messenger *m, int friendId, uint8_t* cName, uint16_t cNameSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendUsernameChanged(friendId, CString::toString(cName, cNameSize));
}

void Core::onStatusMessageChanged(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendStatusMessageChanged(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onUserStatusChanged(Messenger *m, int friendId, USERSTATUS status, void *userdata)
{
    // TODO
}

void Core::onConnectionStatusChanged(Messenger *m, int friendId, uint8_t status, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    if (status) {
        emit core->friendStatusChanged(friendId, FriendStatus::Online);
    }
    else {
        emit core->friendStatusChanged(friendId, FriendStatus::NotFound);
    }
}

void Core::acceptFriendRequest(const QString& userId)
{
    int friendId = m_addfriend_norequest(messenger, CUserId(userId).data());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
        friendIdList << friendId;
    }
}

void Core::requestFriendship(const QString& userId, const QString& message)
{
    CString cMessage(message);

    int friendId = m_addfriend(messenger, CUserId(userId).data(), cMessage.data(), cMessage.size());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
        friendIdList << friendId;
    }
}

void Core::sendMessage(int friendId, const QString& message)
{
    CString cMessage(message);

    if (!m_sendmessage(messenger, friendId, cMessage.data(), cMessage.size())) {
        emit failedToSendMessage(friendId, message);
    }
}

void Core::removeFriend(int friendId)
{
    if (m_delfriend(messenger, friendId) == -1) {
        emit failedToRemoveFriend(friendId);
    } else {
        emit friendRemoved(friendId);
        friendIdList.removeOne(friendId);
    }
}

void Core::setUsername(const QString& username)
{
    CString cUsername(username);

    if (setname(messenger, cUsername.data(), cUsername.size()) == -1) {
        emit failedToSetUsername(username);
    } else {
        emit usernameSet(username);
    }
}

void Core::setStatusMessage(const QString &message)
{
    CString cMessage(message);

    if (m_set_statusmessage(messenger, cMessage.data(), cMessage.size()) == -1) {
        emit failedToSetStatusMessage(message);
    } else {
        emit statusMessageSet(message);
    }
}

void Core::process()
{
    doMessenger(messenger);
#ifdef DEBUG
    //we want to see the debug messages immediately
    fflush(stdout);
#endif
    checkConnection();
}

void Core::bootstrapDht()
{
    const Settings& s = Settings::getInstance();
    QList<Settings::DhtServer> dhtServerList = s.getDhtServerList();

    IP_Port bootstrapIpPort;
    for (const Settings::DhtServer& dhtServer : dhtServerList) {
        bootstrapIpPort.port = htons(dhtServer.port);
        bootstrapIpPort.ip.i = resolve_addr(dhtServer.address.toLatin1().data());
        if (bootstrapIpPort.ip.i == 0) {
            continue;
        }

        DHT_bootstrap(messenger->dht, bootstrapIpPort, CUserId(dhtServer.userId).data());
    }
}

void Core::checkConnection()
{
    static bool isConnected = false;

    if (DHT_isconnected(messenger->dht) && !isConnected) {
        emit connected();
        isConnected = true;
    } else if (!DHT_isconnected(messenger->dht) && isConnected) {
        emit disconnected();
        isConnected = false;
    }
}

void Core::start()
{
    messenger = initMessenger();

    m_callback_friendrequest(messenger, onFriendRequest, this);
    m_callback_friendmessage(messenger, onFriendMessage, this);
    m_callback_namechange(messenger, onFriendNameChange, this);
    m_callback_statusmessage(messenger, onStatusMessageChanged, this);
    m_callback_userstatus(messenger, onUserStatusChanged, this);
    m_callback_connectionstatus(messenger, onConnectionStatusChanged, this);

    QFile bak(MESSENGER_BAK);
    if (bak.open(QIODevice::ReadOnly)) {
        QByteArray arr = bak.readAll();
        Messenger_load(messenger, reinterpret_cast<uint8_t*>(arr.data()), arr.length());
        bak.close();

        for(uint32_t i = 0; i < messenger->numfriends; ++i) {
            QString client_id = CUserId::toString(messenger->friendlist[i].client_id);
            emit friendAdded(i, client_id);

            if(!*messenger->friendlist[i].name)
            {
                continue;
            }
            QString username = CString::toString(messenger->friendlist[i].name);
            emit friendUsernameChanged(i, username);
        }
    }

    uint8_t ourid[FRIEND_ADDRESS_SIZE];
    getaddress(messenger, ourid);
    emit userIdGenerated(CUserId::toString(ourid));

    CString cUsername(Settings::getInstance().getUsername());
    setname(messenger, cUsername.data(), cUsername.size());

    bootstrapDht();

    timer->setInterval(30);
    timer->start();
}

// CUserId

Core::CUserId::CUserId(const QString &userId)
{
    cUserId = new uint8_t[FRIEND_ADDRESS_SIZE];
    cUserIdSize = fromString(userId, cUserId);
}

Core::CUserId::~CUserId()
{
    delete[] cUserId;
}

uint8_t* Core::CUserId::data()
{
    return cUserId;
}

uint16_t Core::CUserId::size()
{
    return cUserIdSize;
}

QString Core::CUserId::toString(const uint8_t* cUserId/*, uint16_t cUserIdSize*/)
{
    return QString(QByteArray(reinterpret_cast<const char*>(cUserId), FRIEND_ADDRESS_SIZE).toHex()).toUpper();
}

uint16_t Core::CUserId::fromString(const QString& userId, uint8_t* cUserId)
{
    QByteArray arr = QByteArray::fromHex(userId.toLower().toLatin1());
    memcpy(cUserId, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}

// CString

Core::CString::CString(const QString& string)
{
    cString = new uint8_t[string.length() * MAX_SIZE_OF_UTF8_ENCODED_CHARACTER + 1]();
    cStringSize = fromString(string, cString);
}

Core::CString::~CString()
{
    delete[] cString;
}

uint8_t* Core::CString::data()
{
    return cString;
}

uint16_t Core::CString::size()
{
    return cStringSize;
}

QString Core::CString::toString(const uint8_t* cString, uint16_t cStringSize)
{
    return QString::fromUtf8(reinterpret_cast<const char*>(cString), cStringSize - 1);
}

QString Core::CString::toString(const uint8_t* cString)
{
    return QString::fromUtf8(reinterpret_cast<const char*>(cString), -1);
}

uint16_t Core::CString::fromString(const QString& string, uint8_t* cString)
{
    QByteArray byteArray = QByteArray(string.toUtf8());
    memcpy(cString, reinterpret_cast<uint8_t*>(byteArray.data()), byteArray.size());
    return byteArray.size() + 1;
}
