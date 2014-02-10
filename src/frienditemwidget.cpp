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
    usernameLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    usernameLabel->setMinimumHeight(usernameLabel->fontMetrics().lineSpacing() + LINE_SPACING_OFFSET);

    statusMessageLabel = new CopyableElideLabel(this);
    statusMessageLabel->setAlignment(Qt::AlignLeft);
    statusMessageLabel->setTextElide(true);
    statusMessageLabel->setTextElideMode(Qt::ElideRight);
    statusMessageLabel->setShowToolTipOnElide(true);
    statusMessageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    statusMessageLabel->setMinimumHeight(usernameLabel->fontMetrics().lineSpacing() + LINE_SPACING_OFFSET);
    QPalette palette;
    palette.setColor(QPalette::Foreground, Qt::gray);
    statusMessageLabel->setPalette(palette);

    QWidget* textWidget = new QWidget(this);
    textWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QVBoxLayout* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 1, 0, 0);
    textLayout->setSpacing(0);
    textLayout->addWidget(usernameLabel);
    textLayout->addWidget(statusMessageLabel);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 1, 2, 0);
    layout->setSpacing(2);
    layout->addWidget(statusLabel, 0, Qt::AlignVCenter);
    layout->addWidget(textWidget);
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
