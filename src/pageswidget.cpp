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
#include "pageswidget.hpp"

#include <QDebug>

PagesWidget::PagesWidget(QWidget* parent) :
    QStackedWidget(parent)
{
    addWidget(new QWidget(this));
}

// if preformance would be critical, replace with QHash<int friendId, ChatPageWidget*>
ChatPageWidget* PagesWidget::widget(int friendId) const
{
    for (int i = 0; i < count(); i ++) {
        ChatPageWidget* chatPage = dynamic_cast<ChatPageWidget*>(QStackedWidget::widget(i));
        if (chatPage != nullptr && chatPage->getFriendId() == friendId) {
            return chatPage;
        }
    }
    return nullptr;
}

void PagesWidget::addPage(int friendId, const QString& username)
{
    ChatPageWidget* chatPage = new ChatPageWidget(friendId, this);
    chatPage->setUsername(username);
    connect(chatPage, &ChatPageWidget::messageSent, this, &PagesWidget::onMessageSent);
    addWidget(chatPage);
    qDebug() << "page" << friendId << "added" << count();
}

void PagesWidget::activatePage(int friendId)
{
    setCurrentWidget(widget(friendId));
}

void PagesWidget::removePage(int friendId)
{
    ChatPageWidget* chatPage = widget(friendId);
    removeWidget(chatPage);
    delete chatPage;
    qDebug() << "page" << friendId << "removed" << count();
}

void PagesWidget::usernameChanged(int friendId, const QString& username)
{
    widget(friendId)->setUsername(username);
}

void PagesWidget::statusChanged(int friendId, Status status)
{
    widget(friendId)->setStatus(status);
}

void PagesWidget::onMessageSent(const QString& message)
{
    ChatPageWidget* chatPage = static_cast<ChatPageWidget*>(sender());
    emit messageSent(chatPage->getFriendId(), message);
}

void PagesWidget::messageReceived(int friendId, const QString &message)
{
    widget(friendId)->messageReceived(message);
}
