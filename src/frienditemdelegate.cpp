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

#include "frienditemdelegate.hpp"
#include "status.hpp"

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPen>

FriendItemDelegate::FriendItemDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

QSize FriendItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QIcon avatarIcon = QIcon(":/icons/buddy.png");
    QSize avatarIconSize = avatarIcon.actualSize(option.decorationSize);

    static const int VERTICAL_PADDING = 2;

    return QSize(avatarIconSize.width(), VERTICAL_PADDING + avatarIconSize.height() + VERTICAL_PADDING);
}

void FriendItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QSize hint = sizeHint(option, index);

    // Avatar Icon
    QIcon avatarIcon = QIcon(":/icons/buddy.png");
    QSize avatarIconSize = avatarIcon.actualSize(option.decorationSize);

    static const int AVATAR_ICON_X_OFFSET = 0;

    painter->drawPixmap(AVATAR_ICON_X_OFFSET, option.rect.top() + (hint.height() - avatarIconSize.height())/2, avatarIcon.pixmap(avatarIconSize.width(), avatarIconSize.height()));

    // Status dot
    QIcon statusDot;
    statusDot.addFile(StatusHelper::getInfo(getStatus(index)).iconPathSmall, QSize(16,16), QIcon::Normal);
    statusDot.addFile(StatusHelper::getInfo(getStatus(index)).iconPathSmallActive, QSize(16,16), QIcon::Active);
    QSize statusDotSize = statusDot.actualSize(option.decorationSize);

    static const int STATUSDOT_X_OFFSET = 2;

    QPoint dotPos = option.rect.center();
    dotPos.setX(option.rect.right()-STATUSDOT_X_OFFSET-statusDotSize.width());

    if (getUnreadMessages(index)) {
        painter->drawPixmap(dotPos - QPoint(0, statusDotSize.height()/2), statusDot.pixmap(statusDotSize.width(), statusDotSize.height(), QIcon::Active));
    } else {
        painter->drawPixmap(dotPos - QPoint(0, statusDotSize.height()/2), statusDot.pixmap(statusDotSize.width(), statusDotSize.height(), QIcon::Normal));
    }

    // Username
    QString username = index.data(UsernameRole).toString();

    QString statusMessage = index.data(StatusMessageRole).toString();
    const bool statusMessageIsVisible = statusMessage.trimmed().length() != 0;

    QFont usernameFont = QApplication::font();

    static const int USERNAME_X_OFFSET = 2;
    static const int USERNAME_Y_OFFSET = -3;

    painter->setFont(usernameFont);
    QString elidedUsername = painter->fontMetrics().elidedText(username, Qt::ElideRight, option.rect.right() - (AVATAR_ICON_X_OFFSET + avatarIconSize.width() + USERNAME_X_OFFSET + STATUSDOT_X_OFFSET*2 + statusDotSize.width()));
    painter->drawText(AVATAR_ICON_X_OFFSET + avatarIconSize.width() + USERNAME_X_OFFSET, option.rect.top() + hint.height()/2 + ((statusMessageIsVisible ? 0 : painter->fontMetrics().ascent()) - painter->fontMetrics().descent())/2 + (statusMessageIsVisible ? USERNAME_Y_OFFSET : 0), elidedUsername);

    if (statusMessageIsVisible) {
        //Status Message
        QFont statusMessageFont = QApplication::font();

        painter->setPen(Qt::gray);

        static const int STATUSMESSAGE_X_OFFSET = USERNAME_X_OFFSET;

        painter->setFont(statusMessageFont);
        QString elidedStatuseMessage = painter->fontMetrics().elidedText(statusMessage, Qt::ElideRight, option.rect.right() - (AVATAR_ICON_X_OFFSET + avatarIconSize.width() + STATUSMESSAGE_X_OFFSET + STATUSDOT_X_OFFSET*2 + statusDotSize.width()));
        painter->drawText(AVATAR_ICON_X_OFFSET + avatarIconSize.width() + STATUSMESSAGE_X_OFFSET, option.rect.top() + hint.height()/2 + painter->fontMetrics().ascent(), elidedStatuseMessage);
    }

    painter->restore();
}

Status FriendItemDelegate::getStatus(const QModelIndex& index)
{
    if (index.data(StatusRole).canConvert<Status>()) {
        return index.data(StatusRole).value<Status>();
    } else {
        qDebug() << "Couldn't convert status for" << index.data(UsernameRole).toString() << "with friendId" << index.data(FriendIdRole).toInt();
        return Status::Offline;
    }
}

QString FriendItemDelegate::getUsername(const QModelIndex& index)
{
    return index.data(UsernameRole).toString();
}

bool FriendItemDelegate::getUnreadMessages(const QModelIndex &index)
{
    return index.data(UnreadMessagesRole).toBool();
}
