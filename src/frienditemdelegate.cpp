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

QSize FriendItemDelegate::sizeHint(const QStyleOptionViewItem&  option, const QModelIndex& index) const
{
    QIcon statusIcon = QIcon(StatusHelper::getInfo(getStatus(index)).iconPath);
    QSize statusIconSize = statusIcon.actualSize(option.decorationSize);

    static const int VERTICAL_PADDING = 2;

    return QSize(statusIconSize.width(), VERTICAL_PADDING + statusIconSize.height() + VERTICAL_PADDING);
}

void FriendItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QSize hint = sizeHint(option, index);

    //Status Icon
    QIcon statusIcon = QIcon(StatusHelper::getInfo(getStatus(index)).iconPath);
    QSize statusIconSize = statusIcon.actualSize(option.decorationSize);

    static const int ICON_X_OFFSET = 0;

    painter->drawPixmap(ICON_X_OFFSET, option.rect.top() + (hint.height() - statusIconSize.height())/2, statusIcon.pixmap(statusIconSize.width(), statusIconSize.height()));


    //Username
    QString username = index.data(UsernameRole).toString();

    QString statusMessage = index.data(StatusMessageRole).toString();
    const bool statusMessageIsVisible = statusMessage.trimmed().length() != 0;

    QFont usernameFont = QApplication::font();

    static const int USERNAME_X_OFFSET = 2;

    painter->setFont(usernameFont);
    QString elidedUsername = painter->fontMetrics().elidedText(username, Qt::ElideRight, option.rect.right() - (ICON_X_OFFSET + statusIconSize.width() + USERNAME_X_OFFSET));
    painter->drawText(ICON_X_OFFSET + statusIconSize.width() + USERNAME_X_OFFSET, option.rect.top() + hint.height()/2 + ((statusMessageIsVisible ? 0 : painter->fontMetrics().ascent()) - painter->fontMetrics().descent())/2, elidedUsername);

    if (statusMessageIsVisible) {
        //Status Message
        QFont statusMessageFont = QApplication::font();

        painter->setPen(Qt::gray);

        static const int STATUSMESSAGE_X_OFFSET = USERNAME_X_OFFSET;

        painter->setFont(statusMessageFont);
        QString elidedStatuseMessage = painter->fontMetrics().elidedText(statusMessage, Qt::ElideRight, option.rect.right() - (ICON_X_OFFSET + statusIconSize.width() + STATUSMESSAGE_X_OFFSET));
        painter->drawText(ICON_X_OFFSET + statusIconSize.width() + STATUSMESSAGE_X_OFFSET, option.rect.top() + hint.height()/2 + painter->fontMetrics().ascent(), elidedStatuseMessage);
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
