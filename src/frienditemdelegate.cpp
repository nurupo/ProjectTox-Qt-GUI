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
    painter->save();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.state |= QStyle::State_Active;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();

    // Background
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    // Avatar Icon
    QIcon avatarIcon = QIcon(":/icons/buddy.png");
    QSize avatarIconSize = avatarIcon.actualSize(opt.decorationSize);

    static const int AVATAR_ICON_X_OFFSET = 0;

    QRect avatarRect = QRect(opt.rect.left() + AVATAR_ICON_X_OFFSET, opt.rect.top() + (opt.rect.height() - avatarIconSize.height())/2, avatarIconSize.width(), avatarIconSize.height());
    style->drawItemPixmap(painter, avatarRect, opt.displayAlignment, avatarIcon.pixmap(avatarIconSize));

    // Status dot
    QIcon dot;
    dot.addFile(StatusHelper::getInfo(getStatus(index)).iconPathSmall, QSize(16,16), QIcon::Normal);
    dot.addFile(StatusHelper::getInfo(getStatus(index)).iconPathSmallActive, QSize(16,16), QIcon::Active);
    QSize dotSize = dot.actualSize(opt.decorationSize);

    static const int DOT_X_OFFSET = 2;

    QRect dotRect = QRect(opt.rect.right() - DOT_X_OFFSET - dotSize.width(), opt.rect.top() + (opt.rect.height() - dotSize.height())/2, dotSize.width(), dotSize.height());
    style->drawItemPixmap(painter, dotRect, opt.decorationAlignment, dot.pixmap(dotSize.width(), dotSize.height(), getUnreadMessages(index) ? QIcon::Active : QIcon::Normal));

    // Username
    QString username = index.data(UsernameRole).toString();
    QString statusMessage = index.data(StatusMessageRole).toString();
    const bool statusMessageIsVisible = statusMessage.trimmed().length() != 0;

    static const int USERNAME_X_OFFSET = 2;
    static const int STATUS_Y_OFFSET = 2;



    QRect nameRect = QRect(AVATAR_ICON_X_OFFSET + avatarIconSize.width() + USERNAME_X_OFFSET,
                           opt.rect.top() + (opt.rect.height() - opt.fontMetrics.height())/2 - (statusMessageIsVisible ? (opt.fontMetrics.height() + STATUS_Y_OFFSET)/2 : 0),
                           opt.rect.width() - AVATAR_ICON_X_OFFSET - avatarIconSize.width() - USERNAME_X_OFFSET - DOT_X_OFFSET - dotSize.width() - DOT_X_OFFSET,
                           opt.fontMetrics.height());
    QString elidedUsername = opt.fontMetrics.elidedText(username, Qt::ElideRight, nameRect.width());



    // I fonud no way, to get a styled active item text color :(
    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    painter->setPen(opt.palette.color(cg, (opt.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text));

    style->drawItemText(painter, nameRect, opt.displayAlignment, opt.palette, (opt.state & QStyle::State_Enabled) ? true : false, elidedUsername);

    // StatusMessage
    if (statusMessageIsVisible) {
        QColor color = painter->pen().color();
        color.setAlpha(128);
        painter->setPen(color);

        QRect statusRect = QRect(nameRect.left(),
                                 nameRect.top() + opt.fontMetrics.height() + STATUS_Y_OFFSET,
                                 nameRect.width(),
                                 nameRect.height());
        QString elidedStatus = opt.fontMetrics.elidedText(statusMessage, Qt::ElideRight, statusRect.width());
        style->drawItemText(painter, statusRect, opt.displayAlignment, opt.palette, (opt.state & QStyle::State_Enabled) ? true : false, elidedStatus);
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
