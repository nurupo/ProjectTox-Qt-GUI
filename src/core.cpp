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

class CUserId
{
public:
    explicit CUserId(const QString& userId);
    ~CUserId();

    uint8_t* data();
    uint16_t size();

    static QString toString(const uint8_t* cUserId/*, uint16_t cUserIdSize*/);

private:
    uint8_t* cUserId;
    uint16_t cUserIdSize;

    static uint16_t fromString(const QString& userId, uint8_t* cUserId);
};


CUserId::CUserId(const QString &userId)
{
    cUserId = new uint8_t[FRIEND_ADDRESS_SIZE];
    cUserIdSize = fromString(userId, cUserId);
}

CUserId::~CUserId()
{
    delete[] cUserId;
}

uint8_t* CUserId::data()
{
    return cUserId;
}

uint16_t CUserId::size()
{
    return cUserIdSize;
}

QString CUserId::toString(const uint8_t* cUserId/*, uint16_t cUserIdSize*/)
{
    return QString(QByteArray(reinterpret_cast<const char*>(cUserId), FRIEND_ADDRESS_SIZE).toHex()).toUpper();
}

uint16_t CUserId::fromString(const QString& userId, uint8_t* cUserId)
{
    QByteArray arr = QByteArray::fromHex(userId.toLower().toLatin1());
    memcpy(cUserId, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}


class CString
{
public:
    explicit CString(const QString& string);
    ~CString();

    uint8_t* data();
    uint16_t size();

    static QString toString(const uint8_t* cMessage, uint16_t cMessageSize);
    static QString toString(const uint8_t* cMessage);


private:
    const static int MAX_SIZE_OF_UTF8_ENCODED_CHARACTER = 4;

    uint8_t* cString;
    uint16_t cStringSize;

    static uint16_t fromString(const QString& message, uint8_t* cMessage);
};

CString::CString(const QString& string)
{
    cString = new uint8_t[string.length() * MAX_SIZE_OF_UTF8_ENCODED_CHARACTER + 1]();
    cStringSize = fromString(string, cString);
}

CString::~CString()
{
    delete[] cString;
}

uint8_t* CString::data()
{
    return cString;
}

uint16_t CString::size()
{
    return cStringSize;
}

QString CString::toString(const uint8_t* cString, uint16_t cStringSize)
{
    return QString::fromUtf8(reinterpret_cast<const char*>(cString), cStringSize - 1);
}

QString CString::toString(const uint8_t* cString)
{
    return QString::fromUtf8(reinterpret_cast<const char*>(cString), -1);
}

uint16_t CString::fromString(const QString& string, uint8_t* cString)
{
    QByteArray byteArray = QByteArray(string.toUtf8());
    memcpy(cString, reinterpret_cast<uint8_t*>(byteArray.data()), byteArray.size());
    return byteArray.size() + 1;
}


class CorePrivate
{
public:
    CorePrivate(Core *core);

    static void onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata);
    static void onFriendMessage(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata);
    static void onFriendNameChange(Messenger *m, int friendId, uint8_t* cName, uint16_t cNameSize, void *userdata);
    static void onStatusMessageChanged(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata);
    static void onUserStatusChanged(Messenger *m, int friendId, USERSTATUS status, void *userdata);
    static void onConnectionStatusChanged(Messenger *m, int friendId, uint8_t status, void *userdata);

    void checkConnection();
    void save();
    void load();

    Messenger *messenger;
    QTimer* timer;
    QList<int> friendIdList;

private:
    Core * const q_ptr;
    Q_DECLARE_PUBLIC(Core);
};

CorePrivate::CorePrivate(Core *core)
    : q_ptr(core)
{

}

void CorePrivate::onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendRequestRecieved(CUserId::toString(cUserId), CString::toString(cMessage, cMessageSize));
}

void CorePrivate::onFriendMessage(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendMessageRecieved(friendId, CString::toString(cMessage, cMessageSize));
}

void CorePrivate::onFriendNameChange(Messenger *m, int friendId, uint8_t* cName, uint16_t cNameSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendUsernameChanged(friendId, CString::toString(cName, cNameSize));
}

void CorePrivate::onStatusMessageChanged(Messenger *m, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    emit core->friendStatusMessageChanged(friendId, CString::toString(cMessage, cMessageSize));
}

void CorePrivate::onUserStatusChanged(Messenger *m, int friendId, USERSTATUS status, void *userdata)
{
    // TODO
}

void CorePrivate::onConnectionStatusChanged(Messenger *m, int friendId, uint8_t status, void *userdata)
{
    Core *core = static_cast<Core*>(userdata);
    if (status) {
        emit core->friendStatusChanged(friendId, Core::FriendStatus::Online);
    }
    else {
        emit core->friendStatusChanged(friendId, Core::FriendStatus::NotFound);
    }
}


void CorePrivate::checkConnection()
{
    Q_Q(Core);
    static bool isConnected = false;

    if (DHT_isconnected(messenger->dht) && !isConnected) {
        emit q->connected();
        isConnected = true;
    } else if (!DHT_isconnected(messenger->dht) && isConnected) {
        emit q->disconnected();
        isConnected = false;
    }
}

void CorePrivate::save()
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

void CorePrivate::load()
{
    Q_Q(Core);

    QFile bak(MESSENGER_BAK);
    if (bak.open(QIODevice::ReadOnly)) {
        QByteArray arr = bak.readAll();
        Messenger_load(messenger, reinterpret_cast<uint8_t*>(arr.data()), arr.length());
        bak.close();

        for(uint32_t i = 0; i < messenger->numfriends; ++i) {
            QString client_id = CUserId::toString(messenger->friendlist[i].client_id);
            emit q->friendAdded(i, client_id);

            if(!*messenger->friendlist[i].name)
            {
                continue;
            }
            QString username = CString::toString(messenger->friendlist[i].name);
            emit q->friendUsernameChanged(i, username);
        }
    }
}

Core::Core() :
    QObject(nullptr),
    d_ptr(new CorePrivate(this))
{
    Q_D(Core);

    qRegisterMetaType<Core::FriendStatus>("FriendStatus");

    d->timer = new QTimer(this);
    connect(d->timer, &QTimer::timeout, this, &Core::process);
    connect(&Settings::getInstance(), &Settings::dhtServerListChanged, this, &Core::bootstrapDht);
}

Core::~Core()
{
    Q_D(Core);

    d->save();

    delete d;
}

void Core::acceptFriendRequest(const QString& userId)
{
    Q_D(Core);
    int friendId = m_addfriend_norequest(d->messenger, CUserId(userId).data());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
        d->friendIdList << friendId;
    }
}

void Core::requestFriendship(const QString& userId, const QString& message)
{
    Q_D(Core);
    CString cMessage(message);

    int friendId = m_addfriend(d->messenger, CUserId(userId).data(), cMessage.data(), cMessage.size());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
        d->friendIdList << friendId;
    }
}

void Core::sendMessage(int friendId, const QString& message)
{
    Q_D(Core);
    CString cMessage(message);

    if (!m_sendmessage(d->messenger, friendId, cMessage.data(), cMessage.size())) {
        emit failedToSendMessage(friendId, message);
    }
}

void Core::removeFriend(int friendId)
{
    Q_D(Core);
    if (m_delfriend(d->messenger, friendId) == -1) {
        emit failedToRemoveFriend(friendId);
    } else {
        emit friendRemoved(friendId);
        d->friendIdList.removeOne(friendId);
    }
}

void Core::setUsername(const QString& username)
{
    Q_D(Core);
    CString cUsername(username);

    if (setname(d->messenger, cUsername.data(), cUsername.size()) == -1) {
        emit failedToSetUsername(username);
    } else {
        emit usernameSet(username);
    }
}

void Core::setStatusMessage(const QString &message)
{
    Q_D(Core);
    CString cMessage(message);

    if (m_set_statusmessage(d->messenger, cMessage.data(), cMessage.size()) == -1) {
        emit failedToSetStatusMessage(message);
    } else {
        emit statusMessageSet(message);
    }
}

void Core::process()
{
    Q_D(Core);
    doMessenger(d->messenger);
#ifdef DEBUG
    //we want to see the debug messages immediately
    fflush(stdout);
#endif
    d->checkConnection();
}

void Core::bootstrapDht()
{
    Q_D(Core);
    const Settings& s = Settings::getInstance();
    QList<Settings::DhtServer> dhtServerList = s.getDhtServerList();

    IP_Port bootstrapIpPort;
    for (const Settings::DhtServer& dhtServer : dhtServerList) {
        bootstrapIpPort.port = htons(dhtServer.port);
        bootstrapIpPort.ip.i = resolve_addr(dhtServer.address.toLatin1().data());
        if (bootstrapIpPort.ip.i == 0) {
            continue;
        }

        DHT_bootstrap(d->messenger->dht, bootstrapIpPort, CUserId(dhtServer.userId).data());
    }
}

void Core::start()
{
    Q_D(Core);

    d->messenger = initMessenger();

    m_callback_friendrequest(d->messenger, CorePrivate::onFriendRequest, this);
    m_callback_friendmessage(d->messenger, CorePrivate::onFriendMessage, this);
    m_callback_namechange(d->messenger, CorePrivate::onFriendNameChange, this);
    m_callback_statusmessage(d->messenger, CorePrivate::onStatusMessageChanged, this);
    m_callback_userstatus(d->messenger, CorePrivate::onUserStatusChanged, this);
    m_callback_connectionstatus(d->messenger, CorePrivate::onConnectionStatusChanged, this);

    d->load();

    uint8_t ourid[FRIEND_ADDRESS_SIZE];
    getaddress(d->messenger, ourid);
    emit userIdGenerated(CUserId::toString(ourid));

    CString cUsername(Settings::getInstance().getUsername());
    setname(d->messenger, cUsername.data(), cUsername.size());

    bootstrapDht();

    d->timer->setInterval(30);
    d->timer->start();
}
