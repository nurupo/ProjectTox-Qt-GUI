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

#include <QThread>
#include <QTime>

Core::Core() :
    tox(nullptr)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Core::process);
    connect(&Settings::getInstance(), &Settings::dhtServerListChanged, this, &Core::bootstrapDht);
}

Core::~Core()
{
    if (tox) {
        tox_kill(tox);
    }
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

void Core::onUserStatusChanged(Tox*/* tox*/, int friendId, TOX_USERSTATUS userstatus, void* core)
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
    tox_set_user_status(tox, userstatus);
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

Core::Profile::Profile(QString filePath)
{
    pPath = filePath;
    if(loadFile() != 0)
        throw 0;
}

Core::Profile::Profile(QString filePath, QString name, QString password)
{
    pPath = filePath;
    pName = name;
    pTox = tox_new(0);

    if (pTox == nullptr)
        throw 0;

    //derive new key for saving
    randombytes(salt, 24);
    randombytes(nonce, 24);
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);
    pLocked = false;
}

Core::Profile::~Profile()
{
    if(!pLocked)
        lock();
}

int Core::Profile::unlock(QString password)
{
    if(!pLocked)
        return 0;

    QFile file(pPath);
    file.open(QFile::ReadOnly);
    uint8_t *rawFile = file.map(0, file.size());

    //derive key from file
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    //load encrypted block
    uint8_t blockTwoEncrypted[blockTwoLength], blockTwoPlaintext[blockTwoLength];
    memcpy(blockTwoEncrypted, rawFile + blockTwoOffset, blockTwoLength);

    //decrypt block
    if(crypto_secretbox_open(blockTwoPlaintext,blockTwoEncrypted,blockTwoLength,nonce,encryptedKey) != 0)
        return -1;

    //check magic
    char magic[4];
    memcpy(magic, rawFile, 36);
    if(strcmp(magic,"rtas") != 0)
        return -1;

    uint8_t messenger[blockTwoLength - 36];
    memcpy(messenger, blockTwoPlaintext + 36, blockTwoLength - 36);

    //create this profile's tox instance
    pTox = tox_new(0);
    if(pTox == nullptr)
        return -1;
    tox_load(pTox, messenger, blockTwoLength - 36);

    //check against loaded scrypt values being too small
    if(scryptN < 15)
        scryptN = 15;
    if(scryptR < 8)
        scryptR = 8;

    /* Generate a new key for future saving.
     * This somewhat more secure than keeping the user password around in plaintext in
     * the ram (keeping in mind, of course, that it's already game-over if the client is
     * compromised). Still though - it's the thought that counts.
     */
    randombytes(salt, 24);
    randombytes(nonce, 24);
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    memset(messenger, 0, blockTwoLength - 36);
    memset(blockTwoPlaintext, 0, blockTwoLength);

    file.unmap(rawFile);
    file.close();

    pLocked = false;

    return 0;
}
int Core::Profile::lock()
{
    if(!pLocked)
        saveFile();
    else
        return -1;

    tox_kill(pTox);
    free(pTox);
    pTox = nullptr;

    memset(encryptedKey,0,32);

    pLocked = true;
    return 0;
}

int Core::Profile::save()
{
    if(!pLocked)
        saveFile();
    else
        return -1;
    return 0;
}

int Core::Profile::changePassword(QString oldPassword, QString newPassword)
{
    if(pLocked)
        if(unlock(oldPassword) != 0)
            return -1;

    uint8_t oldKey[32];
    scrypt((const uint8_t*)oldPassword.toLocal8Bit().constData(), oldPassword.length(), salt, 24, scryptN, scryptR, scryptP, oldKey, 32);

    /* Check to see if keys match.
     * Although there's no cryptographic need to do so, it ensures that whomever's using
     * the profile should have access to it, rather than having stumbled upon an unlocked
     * computer.
     */
    if(strcmp((const char*)oldKey,(const char*)encryptedKey) != 0)
        return -1;

    scrypt((const uint8_t*)newPassword.toLocal8Bit().constData(), newPassword.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    return 0;
}
void changeName(QString newName);


QString Core::Profile::getName()
{
    return pName;
}

QDateTime Core::Profile::getSaveTime()
{
    return pSavedTime;
}

bool Core::Profile::isLocked()
{
    return pLocked;
}

int Core::Profile::loadFile()
{
    QFile file(pPath);
    file.open(QFile::ReadOnly);
    uint8_t *rawFile = file.map(0, file.size());

    //check magic
    char magic[4];
    memcpy(magic, rawFile, 4);
    if(strcmp(magic,"libe") != 0)
        return -1;
    size_t offset = 4;

    //read time last saved
    pSavedTime.fromMSecsSinceEpoch(*(uint64_t*)rawFile+offset);
    offset += 8;

    //read name
    pName.fromLocal8Bit((const char*)rawFile+offset+2,*(uint16_t*)rawFile+offset);
    offset += 2 + *(uint16_t*)rawFile+offset;

    //scrypt vars
    scryptN = *(uint32_t*)(rawFile + offset);
    scryptR = *(uint32_t*)(rawFile + offset + 4);
    scryptP = *(uint32_t*)(rawFile + offset + 8);
    offset += 12;

    //salt & nonce
    memcpy(salt, rawFile + offset, 24);
    memcpy(nonce, rawFile + offset + 24, 24);
    offset += 24;

    //block two
    blockTwoLength = *(uint64_t*)(rawFile + offset);
    offset += 8;
    blockTwoOffset = offset;

    return 0;
}

int Core::Profile::saveFile()
{
    /* Create block two */
    size_t blockTwoSize = tox_size(pTox) + 36, totalSize;
    uint8_t blockTwoPlaintext[blockTwoSize], blockTwoEncrypted[blockTwoSize];
    uint8_t magic1[4] = {0x72, 0x74, 0x61, 0x73};

    memcpy(blockTwoPlaintext + 32, magic1, 4);
    tox_save(pTox, blockTwoPlaintext + 32 + 4);

    /* Encrypt block two */
    if(crypto_secretbox(blockTwoEncrypted, blockTwoPlaintext, blockTwoSize, nonce, encryptedKey) != 0)
        return -1;
    memset(blockTwoPlaintext, 0, blockTwoSize);

    /* Compose entire file */
    //determine file size
    uint16_t nameLength = pName.length();
    totalSize = blockTwoSize + nameLength + 82;

    //resize & mmap file
    QFile file(pPath);
    file.open(QFile::WriteOnly);
    file.resize(totalSize);
    uint8_t *rawFile = file.map(0,file.size());

    //magic
    uint8_t magic2[4] = {0x6c, 0x69, 0x62, 0x65};
    memcpy(rawFile,magic2,4);
    size_t offset = 4;

    //time
    uint64_t saved = time(NULL);
    memcpy(rawFile + offset, &saved, 8);
    offset += 8;
    pSavedTime.fromMSecsSinceEpoch(saved);

    //profile name
    memcpy(rawFile + offset, &nameLength, 2);
    offset += 2;
    memcpy(rawFile + offset, pName.toLocal8Bit().constData(), nameLength);
    offset += nameLength;

    //scrypt values
    memcpy(rawFile + offset, &scryptN, 4);
    offset += 4;
    memcpy(rawFile + offset, &scryptR, 4);
    offset += 4;
    memcpy(rawFile + offset, &scryptP, 4);
    offset += 4;

    //salt & nonce
    memcpy(rawFile + offset, &salt, 24);
    offset += 24;
    memcpy(rawFile + offset, &nonce, 24);
    offset += 24;

    //block two
    memcpy(rawFile + offset, &blockTwoSize, 8);
    offset += 8;
    memcpy(rawFile + offset, &blockTwoEncrypted, blockTwoSize);

    return 0;
}
