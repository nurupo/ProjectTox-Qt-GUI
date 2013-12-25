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
#include <util.h>
#include <sodium.h>

extern "C" {
#include <scrypt-jane.h>
}

#include <QObject>
#include <QTimer>
#include <QList>
#include <QFile>
#include <QDateTime>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ~Core();

private:
    static void onFriendRequest(uint8_t* cUserId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendMessage(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onFriendNameChange(Tox* tox, int friendId, uint8_t* cName, uint16_t cNameSize, void* core);
    static void onStatusMessageChanged(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);
    static void onUserStatusChanged(Tox* tox, int friendId, TOX_USERSTATUS userstatus, void* core);
    static void onConnectionStatusChanged(Tox* tox, int friendId, uint8_t status, void* core);
    static void onAction(Tox* tox, int friendId, uint8_t* cMessage, uint16_t cMessageSize, void* core);

    void checkConnection();

    Tox* tox;
    QTimer* timer;

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
        static QString toString(uint8_t* cMessage);


    private:
        const static int MAX_SIZE_OF_UTF8_ENCODED_CHARACTER = 4;

        uint8_t* cString;
        uint16_t cStringSize;

        static uint16_t fromString(const QString& message, uint8_t* cMessage);
    };

    /* Profile Save Format
     * ==============
     *
     * bytes    name        type        purpose
     * ----------------------------------------
     * -- block one [unencrypted] --
     * 4        magic       uint8       magic,6c:69:62:65 "libe"
     * 8        saved       uint64      timestamp of when the profile was last used
     * 2        namelen     uint16      length of name
     * varies   name        uint8       name of profile
     * 12       scryptvars  uint32      N,r,p variables for scrypt - in this order
     * 24       salt        uint8       the salt for scrypt
     * 24       nonce       uint8       the nonce for nacl
     * 8        blocklen    uint64      the length of the encrypted block
     * -- block two [encrypted] --
     * 32       0           uint8       crypto_secretbox_ZEROBYTES
     * 4        magic       uint8       magic,72:74:61:73 "rtas"
     * varies   profile     uint8       the messenger data - this goes to tox_load()
     */

    class Profile
    {
    public:
        /* Loads a profile from the given path.
         * Throws an exception (0) if the profile can't be loaded.
         */
        Profile(QString filePath);
        /* Creates a new profile.
         * This profile isn't saved to disk until lock() or save() are called.
         */
        Profile(QString filePath, QString name, QString password);
        ~Profile();

        /* Attempts to decrypt the profile.
         * password should be the user input.
         *
         * returns 0 if success
         */
        int unlock(QString password);
        /* Locks the profile, requiring an unlock() to again access.
         * This function also calls save().
         *
         * returns 0 if success
         */
        int lock();
        /* Saves the profile do disk.
         * This does not lock the profile.
         *
         * returns 0 if success
         */
        int save();

        /* Changes the password for the profile.
         *
         * returns 0 if success.
         */
        int changePassword(QString oldPassword, QString newPassword);
        /* Changes the saved name for the profile. */
        void changeName(QString newName);

        /* Gets the profile name, as saved in the file. */
        QString getName();
        /* Gets the last date that the profile was saved, as saved in the file. */
        QDateTime getSaveTime();
        /* Checks if the profile is locked. */
        bool isLocked();

        Tox* pTox = nullptr;
    private:
        int loadFile();
        int saveFile();

        QString pPath;
        bool pLocked = true;

        uint8_t encryptedKey[crypto_secretbox_KEYBYTES], //32 bytes
                nonce[crypto_secretbox_NONCEBYTES], //24 bytes
                salt[24];
        uint32_t scryptN = 15, scryptR = 8, scryptP = 1;

        QString pName;
        QDateTime pSavedTime;

        size_t blockTwoOffset;
        uint64_t blockTwoLength;
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

    void messageSentResult(int friendId, const QString& message, int messageId);
    void actionSentResult(int friendId, const QString& action, int success);

    void failedToAddFriend(const QString& userId);
    void failedToRemoveFriend(int friendId);
    void failedToSetUsername(const QString& username);
    void failedToSetStatusMessage(const QString& message);

    void actionReceived(int friendId, const QString& acionMessage);

    void failedToStart();

};

#endif // CORE_HPP

