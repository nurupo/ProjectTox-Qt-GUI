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

#include <QDebug>
#include <QTime>

Core::Core() :
    tox(nullptr)
{
    connect(&Settings::getInstance(), &Settings::dhtServerListChanged, this, &Core::bootstrapDht);
}

Core::~Core()
{
    if (tox) {
        tox_kill(tox);
    }
}

void Core::onFriendRequest(Tox*/* tox*/, uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void* core)
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

void Core::onUserStatusChanged(Tox*/* tox*/, int friendId, uint8_t userstatus, void* core)
{
    Status status;
    switch (userstatus) {
        case TOX_USERSTATUS_NONE:
            status = Status::Online;
            break;
        case TOX_USERSTATUS_AWAY:
            status = Status::Away;
            break;
        case TOX_USERSTATUS_BUSY:
            status = Status::Busy;
            break;
        default:
            status = Status::Online;
            break;
    }
    emit static_cast<Core*>(core)->friendStatusChanged(friendId, status);
}

void Core::onConnectionStatusChanged(Tox*/* tox*/, int friendId, uint8_t status, void* core)
{
    emit static_cast<Core*>(core)->friendStatusChanged(friendId, status ? Status::Online : Status::Offline);
}

void Core::onAction(Tox*/* tox*/, int friendId, uint8_t *cMessage, uint16_t cMessageSize, void *core)
{
    emit static_cast<Core*>(core)->actionReceived(friendId, CString::toString(cMessage, cMessageSize));
}

void Core::acceptFriendRequest(const QString& userId)
{
    int friendId = tox_add_friend_norequest(tox, CUserId(userId).data());
    if (friendId == -1) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
    }
}

void Core::requestFriendship(const QString& friendAddress, const QString& message)
{
    CString cMessage(message);

    int friendId = tox_add_friend(tox, CFriendAddress(friendAddress).data(), cMessage.data(), cMessage.size());
    const QString userId = friendAddress.mid(0, TOX_CLIENT_ID_SIZE * 2);
    // TODO: better error handling
    if (friendId < 0) {
        emit failedToAddFriend(userId);
    } else {
        emit friendAdded(friendId, userId);
    }
}

void Core::sendMessage(int friendId, const QString& message)
{
    CString cMessage(message);

    int messageId = tox_send_message(tox, friendId, cMessage.data(), cMessage.size());
    emit messageSentResult(friendId, message, messageId);
}

void Core::sendAction(int friendId, const QString &action)
{
    CString cMessage(action);
    int ret = tox_send_action(tox, friendId, cMessage.data(), cMessage.size());
    emit actionSentResult(friendId, action, ret);
}

void Core::removeFriend(int friendId)
{
    if (tox_del_friend(tox, friendId) == -1) {
        emit failedToRemoveFriend(friendId);
    } else {
        emit friendRemoved(friendId);
    }
}

void Core::setUsername(const QString& username)
{
    CString cUsername(username);

    if (tox_set_name(tox, cUsername.data(), cUsername.size()) == -1) {
        emit failedToSetUsername(username);
    } else {
        emit usernameSet(username);
    }
}

void Core::setStatusMessage(const QString& message)
{
    CString cMessage(message);

    if (tox_set_status_message(tox, cMessage.data(), cMessage.size()) == -1) {
        emit failedToSetStatusMessage(message);
    } else {
        emit statusMessageSet(message);
    }
}

void Core::setStatus(Status status)
{
    TOX_USERSTATUS userstatus;
    switch (status) {
        case Status::Online:
            userstatus = TOX_USERSTATUS_NONE;
            break;
        case Status::Away:
            userstatus = TOX_USERSTATUS_AWAY;
            break;
        case Status::Busy:
            userstatus = TOX_USERSTATUS_BUSY;
            break;
        default:
            userstatus = TOX_USERSTATUS_INVALID;
            break;
    }

    if (tox_set_user_status(tox, userstatus) == 0) {
        emit statusSet(status);
    } else {
        emit failedToSetStatus(status);
    }
}

void Core::bootstrapDht()
{
    const Settings& s = Settings::getInstance();
    QList<Settings::DhtServer> dhtServerList = s.getDhtServerList();

    for (const Settings::DhtServer& dhtServer : dhtServerList) {
       tox_bootstrap_from_address(tox, dhtServer.address.toLatin1().data(), 0, htons(dhtServer.port), CUserId(dhtServer.userId).data());
    }
}

void Core::process()
{
#ifdef QT_GUI_CALLS_PER_SECOND
    static int time = QTime::currentTime().second();
    static int time_counter = 1;

    int time_new = QTime::currentTime().second();
    if (time != time_new) {
        qDebug("cps: %02d:%02d:%02d = %d", QTime::currentTime().hour(), QTime::currentTime().minute(), QTime::currentTime().second(), time_counter);
        time_counter = 1;
        time = time_new;
    } else {
        time_counter++;
    }
#endif
    tox_do(tox);
#ifdef DEBUG
    //we want to see the debug messages immediately
    fflush(stdout);
#endif
    checkConnection();
#ifdef QT_GUI_TOX_WAIT
    waitWrapper->run();
#endif
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
    tox = tox_new(0);

    if (tox == nullptr) {
        emit failedToStart();
        return;
    }

    tox_callback_friend_request(tox, onFriendRequest, this);
    tox_callback_friend_message(tox, onFriendMessage, this);
    tox_callback_friend_action(tox, onAction, this);
    tox_callback_name_change(tox, onFriendNameChange, this);
    tox_callback_status_message(tox, onStatusMessageChanged, this);
    tox_callback_user_status(tox, onUserStatusChanged, this);
    tox_callback_connection_status(tox, onConnectionStatusChanged, this);

    uint8_t friendAddress[TOX_FRIEND_ADDRESS_SIZE];
    tox_get_address(tox, friendAddress);

    emit friendAddressGenerated(CFriendAddress::toString(friendAddress));

    CString cUsername(Settings::getInstance().getUsername());
    tox_set_name(tox, cUsername.data(), cUsername.size());

    CString cStatusMessage(Settings::getInstance().getStatusMessage());
    tox_set_status_message(tox, cStatusMessage.data(), cStatusMessage.size());

    bootstrapDht();

#ifdef QT_GUI_TOX_WAIT
    waitWrapper = new WaitWrapper(tox, this);
    connect(waitWrapper, &WaitWrapper::wokeUp, this, &Core::process);
    waitWrapper->setWaitTimeMicroseconds(900000);
    waitWrapper->run();
#else
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Core::process);
    timer->setInterval(45);
    timer->start();
#endif
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
    cString = new uint8_t[string.length() * MAX_SIZE_OF_UTF8_ENCODED_CHARACTER]();
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
    return QString::fromUtf8(reinterpret_cast<char*>(cString), cStringSize);
}

uint16_t Core::CString::fromString(const QString& string, uint8_t* cString)
{
    QByteArray byteArray = QByteArray(string.toUtf8());
    memcpy(cString, reinterpret_cast<uint8_t*>(byteArray.data()), byteArray.size());
    return byteArray.size();
}

#ifdef QT_GUI_TOX_WAIT
WaitWrapper::WaitWrapper(Tox* tox, QObject *parent) : QObject(parent), tox(tox)
{
    data = new uint8_t[tox_wait_data_size()];
    connect(&wait, &Wait::wokeUp, this, &WaitWrapper::onWakeUp);
    thread = new QThread(this);
    wait.moveToThread(thread);
    thread->start();
}

WaitWrapper::~WaitWrapper()
{
    delete data;
    thread->quit();
    thread->wait();
}

long WaitWrapper::getWaitTimeSeconds()
{
    return seconds;
}

long WaitWrapper::getWaitTimeMicroseconds()
{
    return microseconds;
}

void WaitWrapper::setWaitTimeSeconds(long seconds)
{
    setWaitTime(seconds, 0);
}

void WaitWrapper::setWaitTimeMicroseconds(long microseconds)
{
    setWaitTime(0, microseconds);
}

void WaitWrapper::setWaitTime(long seconds, long microseconds)
{
    this->seconds = seconds;
    this->microseconds = microseconds;
}

void WaitWrapper::run()
{
    tox_wait_prepare(tox, data);
    qRegisterMetaType<uint8_t*>("uint8_t*");
    QMetaObject::invokeMethod(&wait, "run", Qt::QueuedConnection, Q_ARG(uint8_t*, data), Q_ARG(long, seconds), Q_ARG(long, microseconds));
}

void WaitWrapper::onWakeUp()
{
    tox_wait_cleanup(tox, data);
    emit wokeUp();
}

Wait::Wait(QObject *parent) : QObject(parent)
{
}

void Wait::run(uint8_t* data, long seconds, long microseconds)
{
    tox_wait_execute(data, seconds, microseconds);
    emit wokeUp();
}
#endif
