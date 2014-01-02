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
#include <QVBoxLayout>

FriendItemWidget::FriendItemWidget(QWidget* parent) :
    QWidget(parent)
{
    statusLabel = new QLabel(this);
    statusLabel->setScaledContents(true);
    statusLabel->setFixedSize(24, 24);

    usernameLabel = new CopyableElideLabel(this);
    usernameLabel->setAlignment(Qt::AlignLeft);
    usernameLabel->setTextElide(true);
    usernameLabel->setTextElideMode(Qt::ElideRight);
    usernameLabel->setShowToolTipOnElide(true);
    usernameLabel->setMinimumSize(10, 10);

    statusMessageLabel = new CopyableElideLabel(this);
    statusMessageLabel->setAlignment(Qt::AlignLeft);
    statusMessageLabel->setTextElide(true);
    statusMessageLabel->setTextElideMode(Qt::ElideRight);
    statusMessageLabel->setShowToolTipOnElide(true);
    statusMessageLabel->setMinimumSize(10, 10);
    QPalette palette;
    palette.setColor(QPalette::Foreground, Qt::gray);
    statusMessageLabel->setPalette(palette);

    QWidget* statusWidget = new QWidget(this);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 4, 0, 0);
    statusLayout->addWidget(statusLabel);

    QWidget* textWidget = new QWidget(this);
    QVBoxLayout* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 2, 0, 0);
    textLayout->setSpacing(2);
    textLayout->addWidget(usernameLabel);
    textLayout->addWidget(statusMessageLabel);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(statusWidget, 0, Qt::AlignTop);
    layout->addWidget(textWidget, 1, Qt::AlignVCenter);
}

void FriendItemWidget::setStatus(Status status)
{
    statusLabel->setPixmap(QPixmap(StatusHelper::getInfo(status).iconPath));
}

void FriendItemWidget::setUsername(const QString& username)
{
    usernameLabel->setText(username);
}

void FriendItemWidget::setStatusMessage(const QString& statusMessage)
{
    statusMessageLabel->setText(statusMessage);
    statusMessageLabel->setVisible(statusMessage.trimmed().length() != 0);
}
