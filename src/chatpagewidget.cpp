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
#include "messagedisplaywidget.hpp"
#include "emoticonmenu.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ChatPageWidget::ChatPageWidget(int friendId, QWidget* parent) :
    QWidget(parent), friendId(friendId)
{
    friendItem = new FriendItemWidget(this);
    display = new MessageDisplayWidget(this);

    input = new InputTextWidget(this);
    connect(input, &InputTextWidget::sendMessage, this, &ChatPageWidget::sendMessage);

    // Create emoticon menu :)
    CustomHintWidget *inputPanel = new CustomHintWidget(this, QSize(10, 10));
    EmoticonMenu *menu = new EmoticonMenu(this);
    emoticonButton = new QToolButton(inputPanel);
    emoticonButton->setPopupMode(QToolButton::InstantPopup);
    emoticonButton->setIcon(QIcon(":/icons/emoticons/emotion_smile.png"));
    emoticonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    emoticonButton->setMenu(menu);
    connect(menu, &EmoticonMenu::insertEmoticon, input, &InputTextWidget::insertHtml);
    connect(&Settings::getInstance(), &Settings::smileyPackChanged, menu, &EmoticonMenu::updateEmoticons);
    connect(&Settings::getInstance(), &Settings::dejavuFontChanged, menu, &EmoticonMenu::updateEmoticons);

    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(0,0,0,0);
    inputLayout->setSpacing(2);
    inputLayout->addWidget(input);
    inputLayout->addWidget(emoticonButton);

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(display);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(0, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(friendItem);
    layout->addWidget(splitter);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 3, 2, 3);
}

int ChatPageWidget::getFriendId() const
{
    return friendId;
}

void ChatPageWidget::messageReceived(const QString& message)
{
    display->appendMessage(username, message, -1, false);
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
    //emoticonButton->setDisabled(newStatus == Status::Offline);

}

void ChatPageWidget::messageSentResult(const QString& message, int messageId)
{
    display->appendMessage(Settings::getInstance().getUsername(), message, messageId, true);
}
