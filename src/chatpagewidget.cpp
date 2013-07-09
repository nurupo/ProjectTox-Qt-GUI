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
#include "status.hpp"

#include <QSplitter>
#include <QVBoxLayout>

ChatPageWidget::ChatPageWidget(const QString& userId, QWidget* parent) :
    QWidget(parent), userId(userId)
{
    friendItem = new FriendItemWidget(this);

    display = new MessageDisplayWidget(this);

    input = new InputTextWidget(this);
    connect(input, &InputTextWidget::messageSent, this, &ChatPageWidget::ourOwnMessage);

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(display);
    splitter->addWidget(input);
    splitter->setStretchFactor(1, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(friendItem);
    layout->addWidget(splitter);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 3, 2, 3);
}

QString ChatPageWidget::getUserId() const
{
    return userId;
}

void ChatPageWidget::ourOwnMessage(const QString& message)
{
    // FIXME: get our Username
    display->showMessage("MyUsername", message);
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
}
