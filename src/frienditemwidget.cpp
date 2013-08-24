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

#include "frienditemwidget.hpp"
#include "elidelabel.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

FriendItemWidget::FriendItemWidget(QWidget* parent) :
    QWidget(parent)
{
    avatarLabel = new QLabel(this);
    avatarLabel->setScaledContents(true);
    avatarLabel->setFixedSize(32, 32);
    avatarLabel->setPixmap(QPixmap("://icons/status_online.png"));

    usernameLabel = new ElideLabel(this);
    usernameLabel->setAlignment(Qt::AlignLeft);
    usernameLabel->setTextElide(true);
    usernameLabel->setTextElideMode(Qt::ElideRight);
    usernameLabel->setMinimumSize(10, 10);
    usernameLabel->setProperty("class", "friendName"); // for CSS styling

    statusmessageLabel = new ElideLabel(this);
    statusmessageLabel->setAlignment(Qt::AlignLeft);
    statusmessageLabel->setTextElide(true);
    statusmessageLabel->setTextElideMode(Qt::ElideRight);
    statusmessageLabel->setMinimumSize(10, 10);
    statusmessageLabel->setForegroundRole(QPalette::Mid);
    statusmessageLabel->setProperty("class", "friendStatusmessage"); // for CSS styling

    QVBoxLayout* nameLayout = new QVBoxLayout;
    nameLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout->setSpacing(1);
    nameLayout->addWidget(usernameLabel);
    nameLayout->addWidget(statusmessageLabel);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 2);
    layout->addWidget(avatarLabel, 0, Qt::AlignVCenter);
    layout->addLayout(nameLayout);
    setLayout(layout);

    setStatus(Status::Offline);
}

void FriendItemWidget::setStatus(Status status)
{
    if(status == Status::Online)
        avatarLabel->setEnabled(true);
    else
        avatarLabel->setDisabled(true);
}

void FriendItemWidget::setUsername(const QString& username)
{
    usernameLabel->setText(username);
}

void FriendItemWidget::setStatusmessage(const QString &message)
{
    statusmessageLabel->setText(message);
    statusmessageLabel->setToolTip(message);
}
