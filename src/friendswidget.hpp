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

#ifndef FRIENDSWIDGET_HPP
#define FRIENDSWIDGET_HPP

#include "customhinttreeview.hpp"
#include "filterwidget.hpp"
#include "friendproxymodel.hpp"
#include "status.hpp"

#include <QHash>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>
#include <QWidget>

class FriendsWidget : public QWidget
{
    Q_OBJECT
public:
    FriendsWidget(QWidget* parent);
    enum {UserIdRole = Qt::UserRole, FriendIdRole, StatusRole};
    void setStatus(int friendId, Status status);
    QString getUsername(int friendId);

private:
    FilterWidget* filterEdit;
    CustomHintTreeView* friendView;
    QStandardItemModel* friendModel;
    FriendProxyModel* friendProxyModel;
    QMenu* friendContextMenu;

    QStandardItem* findFriendItem(int friendId) const;
    void setStatus(QStandardItem* friendItem, Status status);

private slots:
    void onFriendContextMenuRequested(const QPoint& pos);
    void onCopyUserIdActionTriggered();
    void onRemoveFriendActionTriggered();
    void onFriendSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

public slots:
    void onAddFriendButtonClicked();
    void addFriend(int friendId, const QString& userId);
    void removeFriend(int friendId);
    void setUsername(int friendId, const QString& username);

signals:
    void friendAdded(int friendId, const QString& userId);
    void friendStatusChanged(int friendId, Status status);
    void friendRemoved(int friendId);
    void friendSelectionChanged(int friendId);
    void friendRequested(const QString& userId, const QString& message);

};

#endif // FRIENDSWIDGET_HPP
