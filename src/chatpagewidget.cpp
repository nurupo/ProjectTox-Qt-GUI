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

#include "chatpagewidget.hpp"
#include "ouruseritemwidget.hpp"
#include "status.hpp"
#include "Settings/settings.hpp"
#include "customhintwidget.hpp"
#include "emoticonmenu.hpp"

#include "messages/messagemodel.hpp"
#include "messages/chatview.hpp"
#include "messages/messagefilter.hpp"
#include "messages/chatviewsearchwidget.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>

ChatPageWidget::ChatPageWidget(int friendId, QWidget* parent) :
    QWidget(parent), friendId(friendId)
{
    friendItem = new FriendItemWidget(this);

    model = new MessageModel(this);
    filterModel = new MessageFilter(this);
    filterModel->setSourceModel(model);
    chatview = new ChatView(filterModel, this);

    searchWidget = new ChatViewSearchWidget(this);
    searchWidget->setScene(chatview->scene());
    connect(searchWidget, &ChatViewSearchWidget::newCurrentHighlight, chatview, &ChatView::scrollTo);

    input = new InputTextWidget(this);
    connect(input, &InputTextWidget::sendMessage, this, &ChatPageWidget::sendMessage);
    connect(input, &InputTextWidget::sendAction,  this, &ChatPageWidget::sendAction);
    connect(input, &InputTextWidget::sendTyping,  this, &ChatPageWidget::sendTyping);

    // Create emoticon menu :)
    CustomHintWidget *inputPanel = new CustomHintWidget(this, QSize(10, 10));
    EmoticonMenu *menu = new EmoticonMenu(this);
    emoticonButton = new QToolButton(inputPanel);
    emoticonButton->setPopupMode(QToolButton::InstantPopup);
    emoticonButton->setIcon(QIcon(":/icons/emoticons/emotion_smile.png"));
    emoticonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    emoticonButton->setMenu(menu);
    connect(menu, &EmoticonMenu::insertEmoticon, input, &InputTextWidget::insertHtml);

    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(0,0,0,0);
    inputLayout->setSpacing(2);
    inputLayout->addWidget(input);
    inputLayout->addWidget(emoticonButton);

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(chatview);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(0, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(friendItem);
    layout->addWidget(searchWidget);
    layout->addWidget(splitter);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 2, 3);
}

int ChatPageWidget::getFriendId() const
{
    return friendId;
}

void ChatPageWidget::messageReceived(const QString& message)
{
    model->insertNewMessage(message, username, Message::Plain);
}

void ChatPageWidget::setUsername(const QString& newUsername)
{
    username = newUsername;
    friendItem->setUsername(username);
}

void ChatPageWidget::setStatus(Status newStatus)
{
    status = newStatus;
    friendItem->setStatus(status);
    input->setDisabled(newStatus == Status::Offline);
    emoticonButton->setDisabled(newStatus == Status::Offline);
}

void ChatPageWidget::setStatusMessage(const QString& statusMessage)
{
    friendItem->setStatusMessage(statusMessage);
}

void ChatPageWidget::messageSentResult(const QString& message, int messageId)
{
    if(messageId == 0)
        model->insertNewMessage(message, Settings::getInstance().getUsername(), Message::Error, Message::Self);
    else
        model->insertNewMessage(message, Settings::getInstance().getUsername(), Message::Plain, Message::Self);
}

void ChatPageWidget::actionReceived(const QString &message)
{
    model->insertNewMessage(message, username, Message::Action);
}

void ChatPageWidget::actionSentResult(const QString &message)
{
    model->insertNewMessage(message, Settings::getInstance().getUsername(), Message::Action, Message::Self);
}

void ChatPageWidget::onFriendUsernameChanged(const QString &newUsername)
{
    if (username.compare(newUsername) == 0) {
        return;
    }

    model->insertNewMessage(newUsername, username, Message::Nick);
    setUsername(newUsername);
}

void ChatPageWidget::onOurUsernameChanged(const QString &newUsername)
{
    model->insertNewMessage(newUsername, Settings::getInstance().getUsername(), Message::Nick, Message::Self);
}

void ChatPageWidget::onFriendTypingChanged(bool isTyping)
{
    chatview->setTypingNotificationVisible(username, isTyping);
}

void ChatPageWidget::showSearchBar()
{
    searchWidget->show();
    searchWidget->enableSearch();
}
