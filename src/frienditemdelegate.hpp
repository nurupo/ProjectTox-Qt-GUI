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

#ifndef FRIENDITEMDELEGATE_HPP
#define FRIENDITEMDELEGATE_HPP

#include <QStyledItemDelegate>
#include "status.hpp"

class FriendItemDelegate : public QStyledItemDelegate
{
public:
    FriendItemDelegate(QObject *parent = 0);

    enum {UsernameRole = Qt::UserRole, StatusRole, StatusMessageRole, UserIdRole, FriendIdRole, LastSeenRole};

    static Status getStatus(const QModelIndex& index);
    static QString getUsername(const QModelIndex& index);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem&  option, const QModelIndex& index) const Q_DECL_OVERRIDE;

};

#endif // FRIENDITEMDELEGATE_HPP
