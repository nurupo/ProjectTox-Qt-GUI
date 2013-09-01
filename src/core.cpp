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

#ifdef WIN32
    #ifndef WINVER
        //Windows XP
        #define WINVER 0x0501
    #endif
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
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
}

void Core::onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit static_cast<Core*>(core)->friendRequestRecieved(CUserId::toString(cUserId), CString::toString(cMessage, cMessageSize));
}

void Core::onFriendMessage(Tox*/* tox*/, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit static_cast<Core*>(core)->friendMessageRecieved(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onFriendNameChange(Tox*/* tox*/, int friendId, uint8_t* cName, uint16_t cNameSize, void* core)
{
    emit static_cast<Core*>(core)->friendUsernameChanged(friendId, CString::toString(cName, cNameSize));
}

void Core::onStatusMessageChanged(Tox*/* tox*/, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
{
    emit static_cast<Core*>(core)->friendStatusMessageChanged(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::onUserStatusChanged(Tox*/* tox*/, int friendId, TOX_USERSTATUS/* status*/, void* core)
{
    emit static_cast<Core*>(core)->friendStatusChanged(friendId, FriendStatus::Online);
}

void Core::onConnectionStatusChanged(Tox*/* tox*/, int friendId, uint8_t status, void* core)
{
    emit static_cast<Core*>(core)->friendStatusChanged(friendId, status ? FriendStatus::Online : FriendStatus::Confirmed);
}

void Core::acceptFriendRequest(const QString& userId)
{
    int friendId = tox_addfriend_norequest(tox, CUserId(userId).data());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
    }
}

void Core::requestFriendship(const QString& friendAddress, const QString& message)
{
    CString cMessage(message);

    int friendId = tox_addfriend(tox, CFriendAddress(friendAddress).data(), cMessage.data(), cMessage.size());
    // TODO: better error handling
    if (friendId < 0) {
        emit failedToAddFriend(friendAddress);
    } else {
        emit friendAdded(friendId, friendAddress);
    }
}

void Core::sendMessage(int friendId, const QString& message)
{
    CString cMessage(message);

    int messageId = tox_sendmessage(tox, friendId, cMessage.data(), cMessage.size());
    emit messageSentResult(friendId, message, messageId);
}

void Core::removeFriend(int friendId)
{
    if (tox_delfriend(tox, friendId) == -1) {
        emit failedToRemoveFriend(friendId);
    } else {
        emit friendRemoved(friendId);
    }
}

void Core::setUsername(const QString& username)
{
    CString cUsername(username);

    if (tox_setname(tox, cUsername.data(), cUsername.size()) == -1) {
        emit failedToSetUsername(username);
    } else {
        emit usernameSet(username);
    }
}


void Core::setStatusMessage(const QString& /*message*/)
{
/*
    CString cMessage(message);

    if (m_set_statusmessage(cMessage.data(), cMessage.size()) == -1) {
        emit failedToSetStatusMessage(message);
    } else {
        emit statusMessageSet(message);
    }
*/
}

void Core::bootstrapDht()
{
    const Settings& s = Settings::getInstance();
    QList<Settings::DhtServer> dhtServerList = s.getDhtServerList();

    tox_IP_Port bootstrapIpPort;
    for (const Settings::DhtServer& dhtServer : dhtServerList) {
        bootstrapIpPort.port = htons(dhtServer.port);
        bootstrapIpPort.ip.i = resolveAddress(dhtServer.address.toLatin1().data());
        if (bootstrapIpPort.ip.i == 0) {
            continue;
        }

        tox_bootstrap(tox, bootstrapIpPort, CUserId(dhtServer.userId).data());
    }
}

uint32_t Core::resolveAddress(const char* address) const
{
    struct addrinfo* server = NULL;
    struct addrinfo hints;
    int rc;
    uint32_t addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only right now.
    hints.ai_socktype = SOCK_DGRAM; // type of socket Tox uses.

    rc = getaddrinfo(address, "echo", &hints, &server);

    // Lookup failed.
    if (rc != 0) {
        return 0;
    }

    // IPv4 records only..
    if (server->ai_family != AF_INET) {
        freeaddrinfo(server);
        return 0;
    }

    addr = ((struct sockaddr_in*)server->ai_addr)->sin_addr.s_addr;

    freeaddrinfo(server);
    return addr;
}

void Core::process()
{
    tox_do(tox);
#ifdef DEBUG
    //we want to see the debug messages immediately
    fflush(stdout);
#endif
    checkConnection();
}

void Core::checkConnection()
{
    static bool isConnected = false;

    if (tox_isconnected(tox) && !isConnected) {
        emit connected();
        isConnected = true;
    } else if (!tox_isconnected(tox) && isConnected) {
        emit disconnected();
        isConnected = false;
    }
}

void Core::start()
{
    tox = tox_new();

    tox_callback_friendrequest(tox, onFriendRequest, this);
    tox_callback_friendmessage(tox, onFriendMessage, this);
    tox_callback_namechange(tox, onFriendNameChange, this);
    //tox_callback_statusmessage(tox, onStatusMessageChanged, this);
    tox_callback_userstatus(tox, onUserStatusChanged, this);
    tox_callback_connectionstatus(tox, onConnectionStatusChanged, this);

    uint8_t friendAddress[TOX_FRIEND_ADDRESS_SIZE];
    tox_getaddress(tox, friendAddress);

    emit friendAddressGenerated(CFriendAddress::toString(friendAddress));

    CString cUsername(Settings::getInstance().getUsername());
    tox_setname(tox, cUsername.data(), cUsername.size());

    bootstrapDht();

    timer->setInterval(30);
    timer->start();
}


// CData

Core::CData::CData(const QString &data, uint16_t byteSize)
{
    cData = new uint8_t[byteSize];
    cDataSize = fromString(data, cData);
}

Core::CData::~CData()
{
    delete[] cData;
}

uint8_t* Core::CData::data()
{
    return cData;
}

uint16_t Core::CData::size()
{
    return cDataSize;
}

QString Core::CData::toString(uint8_t* cData, uint16_t cDataSize)
{
    return QString(QByteArray(reinterpret_cast<char*>(cData), cDataSize).toHex()).toUpper();
}

uint16_t Core::CData::fromString(const QString& data, uint8_t* cData)
{
    QByteArray arr = QByteArray::fromHex(data.toLower().toLatin1());
    memcpy(cData, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}


// CUserId

Core::CUserId::CUserId(const QString &userId) :
    CData(userId, SIZE)
{
    // intentionally left empty
}

QString Core::CUserId::toString(uint8_t* cUserId)
{
    return CData::toString(cUserId, SIZE);
}


// CFriendAddress

Core::CFriendAddress::CFriendAddress(const QString &friendAddress) :
    CData(friendAddress, SIZE)
{
    // intentionally left empty
}

QString Core::CFriendAddress::toString(uint8_t* cFriendAddress)
{
    return CData::toString(cFriendAddress, SIZE);
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
