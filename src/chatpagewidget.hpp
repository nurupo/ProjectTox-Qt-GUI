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

#ifndef CHATPAGEWIDGET_HPP
#define CHATPAGEWIDGET_HPP

#include "frienditemwidget.hpp"
#include "inputtextwidget.hpp"
#include "filetransferstate.hpp"

#include <tox.h>

#include <QFileDialog>
#include <QMap>
#include <QMessageBox>
#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>

class MessageDisplayWidget;
class QToolButton;

class ChatPageWidget : public QWidget
{
    Q_OBJECT
public:
    ChatPageWidget(int friendId, QWidget* parent = 0);
    int getFriendId() const;
    void setUsername(const QString& username);
    void setStatus(Status status);
    void setStatusMessage(const QString& statusMessage);

private:
    FriendItemWidget* friendItem;
    MessageDisplayWidget *display;
    QMap<quint8, FileTransferState*> states;

    InputTextWidget* input;
    QToolButton *filesendButton;
    QToolButton *emoticonButton;

    int friendId;
    QString username;
    Status status;
    QString currentSendFilename;

public slots:
    void messageReceived(const QString& message);
    void messageSentResult(const QString& message, int messageId);
    void actionReceived(const QString& message);
    void actionSentResult(const QString& message);
    quint8 fileSendReceived(quint8 filenumber, quint64 filesize, const QString& filename);
    void fileControlReceived(unsigned int receive_send, quint8 filenumber, quint8 control_type, const QByteArray& data);
    void fileDataReceived(quint8 filenumber, const QByteArray& data);
    void fileSendCompletedReceived(int filenumber);
    void promptSendFile(void);

signals:
    void sendMessage(const QString& message);
    void sendAction(const QString& action);
    void sendFile(FileTransferState* state);
    void sendFileRequest(const QString& filename);
};

#endif // CHATPAGEWIDGET_HPP
