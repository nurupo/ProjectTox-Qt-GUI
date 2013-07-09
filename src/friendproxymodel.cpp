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


#include "friendproxymodel.hpp"
#include "friendswidget.hpp"

#include <QModelIndex>
#include <QDebug>

FriendProxyModel::FriendProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
}

bool FriendProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    Status leftStatus = getStatus(left);
    QString leftName = left.data().toString();

    Status rightStatus = getStatus(right);
    QString rightName = right.data().toString();

    if (leftStatus == rightStatus) {
        return leftName.localeAwareCompare(rightName) > 0;
    } else {
        return leftStatus > rightStatus;
    }
}

Status FriendProxyModel::getStatus(const QModelIndex& index) const
{
    if (index.data(FriendsWidget::StatusRole).canConvert<Status>()) {
        qDebug() << "Converted status for" << index.data().toString();
        return index.data(FriendsWidget::StatusRole).value<Status>();
    } else {
        qDebug() << "Couldn't convert status for" << index.data().toString();
        return Status::Offline;
    }
}
