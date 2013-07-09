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

#include <QHBoxLayout>
#include <QDebug>

FriendItemWidget::FriendItemWidget(QWidget* parent) :
    QWidget(parent)
{
    statusLabel = new QLabel(this);
    statusLabel->setScaledContents(true);
    statusLabel->setFixedSize(24, 24);

    usernameLabel = new QLabel(this);
    usernameLabel->setAlignment(Qt::AlignLeft);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 2);
    layout->addWidget(statusLabel, 0, Qt::AlignVCenter);
    layout->addWidget(usernameLabel, 0, Qt::AlignVCenter);
}

void FriendItemWidget::setStatus(Status status)
{
    statusLabel->setPixmap(QPixmap(StatusHelper::getInfo(status).iconPath));
}

void FriendItemWidget::setUsername(const QString& username)
{
    usernameLabel->setText(username);
}
