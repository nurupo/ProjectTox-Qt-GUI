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
#include "soundmanager.hpp"

#include <QAction>
#include <QClipboard>
#include <QGuiApplication>
#include <QHeaderView>
#include <QVBoxLayout>

FriendsWidget::FriendsWidget(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 0, 2);

    QAction* copyUserIdAction = new QAction(QIcon(":/icons/page_copy.png"), "Copy User ID", this);
    connect(copyUserIdAction, &QAction::triggered, this, &FriendsWidget::onCopyUserIdActionTriggered);

    QAction* removeFriendAction = new QAction(QIcon(":/icons/user_delete.png"), "Remove", this);
    connect(removeFriendAction, &QAction::triggered, this, &FriendsWidget::onRemoveFriendActionTriggered);

    friendContextMenu = new QMenu(this);
    friendContextMenu->addActions(QList<QAction*>() << copyUserIdAction << removeFriendAction);

    friendView = new QTreeView(this);
    friendView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    friendView->setIconSize(QSize(32, 32));
    friendView->setSortingEnabled(true);
    friendView->setIndentation(0);
    friendView->setHeaderHidden(true);
    friendView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    friendView->setItemDelegateForColumn(0, new FriendItemDelegate(this));
    friendView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(friendView, &QTreeView::customContextMenuRequested, this, &FriendsWidget::onFriendContextMenuRequested);

    friendModel = new QStandardItemModel(this);

    friendProxyModel = new FriendProxyModel(this);
    friendProxyModel->setFilterRole(FriendItemDelegate::UsernameRole);
    friendProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    friendProxyModel->setSourceModel(friendModel);

    friendView->setModel(friendProxyModel);
    connect(friendView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FriendsWidget::onFriendSelectionChanged);

    filterEdit = new FilterWidget(this);
    filterEdit->setPlaceholderText("Search");
    filterEdit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    connect(filterEdit, &FilterWidget::textChanged, friendProxyModel, &FriendProxyModel::setFilterFixedString);

    layout->addWidget(filterEdit);
    layout->addWidget(friendView);
}

void FriendsWidget::addFriend(int friendId, const QString& userId)
{
    QStandardItem* item = new QStandardItem();
    item->setData(userId, FriendItemDelegate::UsernameRole);
    item->setData(userId, FriendItemDelegate::UserIdRole);
    item->setData(QString("User ID: %1").arg(userId), Qt::ToolTipRole);
    item->setData(friendId, FriendItemDelegate::FriendIdRole);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    emit friendAdded(friendId, userId);

    Status status = Status::Offline;
    setStatus(item, status);

    friendModel->appendRow(item);
}

void FriendsWidget::setStatus(int friendId, Status status)
{
    QStandardItem* friendItem = findFriendItem(friendId);

    if (friendItem == nullptr) {
        return;
    }

    Status oldStatus = friendItem->data(FriendItemDelegate::StatusRole).value<Status>();
    if (oldStatus == Status::Offline && status == Status::Online)
        SoundManager::getInstance().playSingleSound(Sound::FriendLogsIn);
    else if(status == Status::Offline)
        SoundManager::getInstance().playSingleSound(Sound::FriendLogsOut);

    setStatus(friendItem, status);
}

void FriendsWidget::setStatus(QStandardItem* friendItem, Status status)
{
    friendItem->setData(QVariant::fromValue(status), FriendItemDelegate::StatusRole);

    int friendId = friendItem->data(FriendItemDelegate::FriendIdRole).toInt();

    emit friendStatusChanged(friendId, status);
}

void FriendsWidget::setUsername(int friendId, const QString& username)
{
    QStandardItem* friendItem = findFriendItem(friendId);

    if (friendItem == nullptr) {
        return;
    }

    friendItem->setData(username, FriendItemDelegate::UsernameRole);
}


void FriendsWidget::setStatusMessage(int friendId, const QString& statusMessage)
{
    QStandardItem* friendItem = findFriendItem(friendId);

    if (friendItem == nullptr) {
        return;
    }

    friendItem->setData(statusMessage, FriendItemDelegate::StatusMessageRole);
}

QStandardItem* FriendsWidget::findFriendItem(int friendId) const
{
    QModelIndexList indexList = friendModel->match(friendModel->index(0, 0), FriendItemDelegate::FriendIdRole, friendId);
    if (indexList.size() == 1) {
        return friendModel->itemFromIndex(indexList.at(0));
    }

    return nullptr;
}

void FriendsWidget::onFriendContextMenuRequested(const QPoint& pos)
{
    QPoint globalPos = friendView->viewport()->mapToGlobal(pos);
    globalPos.setX(globalPos.x() + 1);
    QModelIndexList selectedIndexes = friendView->selectionModel()->selectedIndexes();
    if (selectedIndexes.size() != 1) {
        return;
    }
    friendContextMenu->exec(globalPos);
}

void FriendsWidget::onCopyUserIdActionTriggered()
{
    // friendContextMenuRequested already made sure that there is only one index selected
    QModelIndex selectedIndex = friendView->selectionModel()->selectedIndexes().at(0);
    QGuiApplication::clipboard()->setText(friendProxyModel->mapToSource(selectedIndex).data(FriendItemDelegate::UserIdRole).toString());
}

void FriendsWidget::onRemoveFriendActionTriggered()
{
    QModelIndex selectedIndex = friendView->selectionModel()->selectedIndexes().at(0);

    int friendId = friendProxyModel->mapToSource(selectedIndex).data(FriendItemDelegate::FriendIdRole).toInt();

    emit friendRemoved(friendId);
}

void FriendsWidget::removeFriend(int friendId)
{
    QStandardItem* friendItem = findFriendItem(friendId);

    if (friendItem == nullptr) {
        return;
    }

    qDeleteAll(friendModel->takeRow(friendItem->row()));
}

void FriendsWidget::onFriendSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    QStandardItem* item = friendModel->itemFromIndex(friendProxyModel->mapToSource(current));
    if (item != nullptr) {
        emit friendSelectionChanged(item->data(FriendItemDelegate::FriendIdRole).toInt());
    }
}

QString FriendsWidget::getUsername(int friendId)
{
    QStandardItem* friendItem = findFriendItem(friendId);

    if (friendItem == nullptr) {
        return QString();
    }

    return friendItem->text();
}
