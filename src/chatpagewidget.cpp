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

#include "messagedisplaywidget2.hpp"
#include "emoticonmenu.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ChatPageWidget::ChatPageWidget(int friendId, QWidget* parent) :
    QWidget(parent), friendId(friendId)
{
    friendItem = new FriendItemWidget(this);
    display2 = new MessageDisplayWidget2(this);

    input = new InputTextWidget(this);
    connect(input, &InputTextWidget::sendMessage, this, &ChatPageWidget::sendMessage);

    // Create emoticon menu :)
    QWidget *inputPanel = new QWidget(this);
    EmoticonMenu *menu = new EmoticonMenu(this);
    QToolButton *emoticonButton = new QToolButton(inputPanel);
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
    splitter->addWidget(display2);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(1, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(friendItem);
    layout->addWidget(splitter);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 3, 2, 3);

    // give all smileys whith their textual representations to the input dialog for desmiling
}

int ChatPageWidget::getFriendId() const
{
    return friendId;
}

void ChatPageWidget::messageReceived(const QString& message)
{
    display2->appendMessage(username, message);
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
    //input->setReadOnly(newStatus != Status::Online);
}

void ChatPageWidget::MessageSentResult(const QString& message, int messageId)
{
    display2->appendMessage(Settings::getInstance().getUsername(), message, messageId);
}
