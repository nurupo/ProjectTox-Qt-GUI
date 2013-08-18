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

#include "messagedisplaywidget2.h"
#include "emoticonmenu.h"

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
    connect(input, &InputTextWidget::messageSent, this, &ChatPageWidget::messageSent);
    connect(input, &InputTextWidget::messageSent, this, &ChatPageWidget::onMessageSent);

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
    input->setSmileyList(menu->getSmileyList());
    display2->setSmileyList(menu->getSmileyList());
}

int ChatPageWidget::getFriendId() const
{
    return friendId;
}

void ChatPageWidget::onMessageSent(const QString& message)
{
    display2->appendMessage(Settings::getInstance().getUsername(), message);
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

void ChatPageWidget::failedToSendMessage(const QString& message)
{
    display2->appendErrorMessage(tr("Failed to send message: '%1'").arg(message));
}
