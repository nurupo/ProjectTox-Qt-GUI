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

#include <Messenger.h>

class CorePrivate;

class Core : public QObject
{
    Q_OBJECT
    Q_ENUMS(FriendStatus)

public:
    explicit Core();
    virtual ~Core();

    enum class FriendStatus {NotFound = 0, Added, RequestSent, Confirmed, Online};

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

    void failedToAddFriend(const QString& userId);
    void failedToSendMessage(int friendId, const QString& message);
    void failedToRemoveFriend(int friendId);
    void failedToSetUsername(const QString& username);
    void failedToSetStatusMessage(const QString& message);

private:
    CorePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Core);
};

Q_DECLARE_METATYPE(Core::FriendStatus)

#endif // CORE_HPP

