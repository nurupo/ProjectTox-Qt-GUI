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
#include "frienditemdelegate.hpp"

#include <QModelIndex>
#include <QDebug>

FriendProxyModel::FriendProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
}

bool FriendProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    Status leftStatus = FriendItemDelegate::getStatus(left);
    QString leftName = left.data().toString();

    Status rightStatus = FriendItemDelegate::getStatus(right);
    QString rightName = right.data().toString();

    if (leftStatus == rightStatus) {
        return leftName.localeAwareCompare(rightName) > 0;
    } else {
        return leftStatus > rightStatus;
    }
}
