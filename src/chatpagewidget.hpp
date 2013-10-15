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

private:
    FriendItemWidget* friendItem;
    MessageDisplayWidget *display;

    InputTextWidget* input;
    QToolButton *emoticonButton;

    int friendId;
    QString username;
    Status status;

public slots:
    void messageReceived(const QString& message);
    void messageSentResult(const QString& message, int messageId);
    void actionReceived(const QString& message);
    void actionSentResult(const QString& message);

signals:
    void sendMessage(const QString& message);
    void sendAction(const QString& action);

};

#endif // CHATPAGEWIDGET_HPP
