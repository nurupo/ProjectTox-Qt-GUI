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

#ifndef PAGESWIDGET_HPP
#define PAGESWIDGET_HPP

#include "chatpagewidget.hpp"

#include <QStackedWidget>
#include <QFile>
#include <QObject>

class PagesWidget : public QStackedWidget
{
    Q_OBJECT
public:
    PagesWidget(QWidget* parent);

private:
    ChatPageWidget* widget(int friendId) const;

signals:
    void sendFile(int friendId, FileTransferState* state);
    void fileSendReply(int friendId, quint8 filenumber, quint8 message_id);

private slots:
    void onMessageSent(const QString& message);
    void onActionToSend(const QString& action);
    void onFileToSend(FileTransferState* state);
    void onFileRequestToSend(const QString& filename);

public slots:
    void addPage(int friendId, const QString& username);
    void removePage(int friendId);
    void activatePage(int friendId);
    void statusChanged(int friendId, Status status);
    void usernameChanged(int friendId, const QString& username);
    void statusMessageChanged(int friendId, const QString& statusMessage);
    void messageReceived(int friendId, const QString& message);
    void actionReceived(int friendId, const QString& message);

    void messageSentResult(int friendId, const QString& message, int messageId);
    void actionResult(int friendId, const QString &action, int success);

    void fileSendRequestReceived(int friendId, quint8 filenumber, quint64 filesize, const QString& filename);
    void fileControl(int friendId, unsigned int receive_send, quint8 filenumber, quint8 control_type, const QByteArray& data);
    void fileData(int friendId, quint8 filenumber, const QByteArray& data);
    void fileSendCompleted(int friendId, int filenumber);

signals:
    void fileSendRequest(int friendId, const QString& filename);
    void sendMessage(int friendId, const QString& message);
    void sendAction(int friendId, const QString& action);

};

#endif // PAGESWIDGET_HPP
