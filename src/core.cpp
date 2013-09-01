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

#ifndef WIN32
#include <arpa/inet.h>
#endif

#include <QThread>
#include <QTime>

Core::Core() :
    QObject(nullptr)
{
    qRegisterMetaType<Core::FriendStatus>("FriendStatus");
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Core::process);
    connect(&Settings::getInstance(), &Settings::dhtServerListChanged, this, &Core::bootstrapDht);

    handle = tox_new();
}

Core::~Core() {
    tox_kill(handle);
}

void Core::onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit ((Core*)core)->friendRequestRecieved(CUserId::toString(cUserId), CString::toString(cMessage, cMessageSize));
}

void Core::onFriendMessage(Tox*, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit ((Core*)core)->friendMessageRecieved(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onFriendNameChange(Tox*, int friendId, uint8_t* cName, uint16_t cNameSize, void* core)
{
    emit ((Core*)core)->friendUsernameChanged(friendId, CString::toString(cName, cNameSize));
}

void Core::onStatusMessageChanged(Tox*, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit ((Core*)core)->friendStatusMessageChanged(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onFriendStatusChanged(Tox*, int friendId, TOX_USERSTATUS status, void* core)
{
    emit ((Core*)core)->friendStatusChanged(friendId, static_cast<FriendStatus>(status));
}

void Core::acceptFriendRequest(const QString& userId)
{
    int friendId = tox_addfriend_norequest(handle, CUserId(userId).data());
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

    int friendId = tox_addfriend(handle, CUserId(userId).data(), cMessage.data(), cMessage.size());
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

    int messageId = tox_sendmessage(handle, friendId, cMessage.data(), cMessage.size());
    emit messageSentResult(friendId, message, messageId);

}

void Core::removeFriend(int friendId)
{
    if (tox_delfriend(handle, friendId) == -1) {
        emit failedToRemoveFriend(friendId);
    } else {
        emit friendRemoved(friendId);
        friendIdList.removeOne(friendId);
    }
}

void Core::setUsername(const QString& username)
{
    CString cUsername(username);

    if (tox_setname(handle, cUsername.data(), cUsername.size()) == -1) {
        emit failedToSetUsername(username);
    } else {
        emit usernameSet(username);
    }
}

void Core::setStatusMessage(const QString &message)
{
    CString cMessage(message);

    if (tox_set_statusmessage(handle, cMessage.data(), cMessage.size()) == -1) {
        emit failedToSetStatusMessage(message);
    } else {
        emit statusMessageSet(message);
    }
}

void Core::process()
{
    tox_do(handle);
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

    tox_IP_Port bootstrapIpPort;
    for (const Settings::DhtServer& dhtServer : dhtServerList) {
        bootstrapIpPort.port = htons(dhtServer.port);
        //FIXME this is not in the api
        // tox_resolve_addr(dhtServer.address.toLatin1().data());
        bootstrapIpPort.ip.i = 0x58DFAF42;
        if (bootstrapIpPort.ip.i == 0) {
            continue;
        }

        tox_bootstrap(handle, bootstrapIpPort, CUserId(dhtServer.userId).data());
    }
}

void Core::checkConnection()
{
    static bool isConnected = false;

    if (!isConnected && tox_isconnected(handle)) {
        emit connected();
        isConnected = true;
    } else if (isConnected && !tox_isconnected(handle)) {
        emit disconnected();
        isConnected = false;
    }
}

void Core::start()
{
    tox_callback_friendrequest(handle, onFriendRequest, (void*)this);
    tox_callback_friendmessage(handle, onFriendMessage, (void*)this);
    tox_callback_namechange(handle, onFriendNameChange, (void*)this);
    tox_callback_statusmessage(handle, onStatusMessageChanged, (void*)this);
    tox_callback_userstatus(handle, onFriendStatusChanged, (void*)this);

    uint8_t self_public_key [TOX_FRIEND_ADDRESS_SIZE];
    tox_getaddress(handle, self_public_key);

    emit userIdGenerated(CUserId::toString(self_public_key));

    CString cUsername(Settings::getInstance().getUsername());
    tox_setname(handle, cUsername.data(), cUsername.size());

    bootstrapDht();

    timer->setInterval(30);
    timer->start();
}

// CUserId

Core::CUserId::CUserId(const QString &userId)
{
    cUserId = new uint8_t[TOX_FRIEND_ADDRESS_SIZE];
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

QString Core::CUserId::toString(uint8_t* cUserId/*, uint16_t cUserIdSize*/)
{
    return QString(QByteArray(reinterpret_cast<char*>(cUserId), TOX_FRIEND_ADDRESS_SIZE).toHex()).toUpper();
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

QString Core::CString::toString(uint8_t* cString, uint16_t cStringSize)
{
    return QString::fromUtf8(reinterpret_cast<char*>(cString), cStringSize - 1);
}

QString Core::CString::toString(uint8_t* cString)
{
    return QString::fromUtf8(reinterpret_cast<char*>(cString), -1);
}

uint16_t Core::CString::fromString(const QString& string, uint8_t* cString)
{
    QByteArray byteArray = QByteArray(string.toUtf8());
    memcpy(cString, reinterpret_cast<uint8_t*>(byteArray.data()), byteArray.size());
    return byteArray.size() + 1;
}
