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
    static const int USERNAME_Y_OFFSET = -3;
    static const int STATUSDOT_SPACE = 24;

    painter->setFont(usernameFont);
    QString elidedUsername = painter->fontMetrics().elidedText(username, Qt::ElideRight, option.rect.right() - (ICON_X_OFFSET + statusIconSize.width() + USERNAME_X_OFFSET + STATUSDOT_SPACE));
    painter->drawText(ICON_X_OFFSET + statusIconSize.width() + USERNAME_X_OFFSET, option.rect.top() + hint.height()/2 + ((statusMessageIsVisible ? 0 : painter->fontMetrics().ascent()) - painter->fontMetrics().descent())/2 + (statusMessageIsVisible ? USERNAME_Y_OFFSET : 0), elidedUsername);

    if (statusMessageIsVisible) {
        //Status Message
        QFont statusMessageFont = QApplication::font();

        painter->setPen(Qt::gray);

        static const int STATUSMESSAGE_X_OFFSET = USERNAME_X_OFFSET;

        painter->setFont(statusMessageFont);
        QString elidedStatuseMessage = painter->fontMetrics().elidedText(statusMessage, Qt::ElideRight, option.rect.right() - (ICON_X_OFFSET + statusIconSize.width() + STATUSMESSAGE_X_OFFSET + STATUSDOT_SPACE));
        painter->drawText(ICON_X_OFFSET + statusIconSize.width() + STATUSMESSAGE_X_OFFSET, option.rect.top() + hint.height()/2 + painter->fontMetrics().ascent(), elidedStatuseMessage);
    }

    // Status dot
    QPoint dotPos = option.rect.center();
    dotPos.setX(option.rect.right()-STATUSDOT_SPACE/2);
    painter->setRenderHint(QPainter::Antialiasing);

    switch (getStatus(index)) {
    case Status::Away:
    case Status::Busy:
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#CEBE44"));
        painter->drawChord(dotPos.x()-4, dotPos.y()-4, 8, 8, 2880, 2880);
        painter->setPen(QPen(QColor("#CEBE44"), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(dotPos, 4, 4);
        if (getUnreadMessages(index)) {
            painter->setPen(QPen(QColor("#CEBE44"), 1.5));
            painter->drawEllipse(dotPos, 7, 7);
        }
        break;
    case Status::Offline:
        painter->setPen(QPen(QColor("#C74F50"), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(dotPos, 4, 4);
        if (getUnreadMessages(index)) {
            painter->setPen(QPen(QColor("#C74F50"), 1.5));
            painter->drawEllipse(dotPos, 7, 7);
        }
        break;
    case Status::Online:
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#6BC160"));
        painter->drawEllipse(dotPos, 5, 5);
        if (getUnreadMessages(index)) {
            painter->setPen(QPen(QColor("#6BC160"),1.5));
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(dotPos, 7, 7);
        }
        break;
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
