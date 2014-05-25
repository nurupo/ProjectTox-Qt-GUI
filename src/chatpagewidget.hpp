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
#include "messages/id.hpp"

#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>

class MessageModel;
class MessageFilter;
class ChatView;
class QToolButton;
class ChatViewSearchWidget;

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

    MessageModel *model;
    MessageFilter *filterModel;
    ChatView *chatview;
    ChatViewSearchWidget *searchWidget;

    InputTextWidget* input;
    QToolButton *emoticonButton;

    int friendId;
    QString username;
    Status status;
    MsgId mIsTypingMsg;

public slots:
    void messageReceived(const QString& message);
    void messageSentResult(const QString& message, int messageId);
    void actionReceived(const QString& message);
    void actionSentResult(const QString& message);

    void onFriendUsernameChanged(const QString &newUsername);
    void onOurUsernameChanged(const QString &newUsername);
    void onFriendTypingChanged(bool isTyping);

    void showSearchBar();

signals:
    void sendMessage(const QString& message);
    void sendAction(const QString& action);
    void sendTyping(bool typing);
};

#endif // CHATPAGEWIDGET_HPP
