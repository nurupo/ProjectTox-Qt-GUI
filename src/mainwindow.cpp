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

#include "mainwindow.hpp"
#include "pageswidget.hpp"

#include "friendrequestdialog.hpp"
#include "dhtdialog.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>

#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    const int screenWidth = QApplication::desktop()->width();
    const int screenHeight = QApplication::desktop()->height();
    const int appWidth = 500;
    const int appHeight = 300;

    setGeometry((screenWidth - appWidth) / 2, (screenHeight - appHeight) / 2, appWidth, appHeight);

    setWindowTitle("developers' test version, not for public use");

    QMenuBar* menu = new QMenuBar(this);
    setMenuBar(menu);
    setContextMenuPolicy(Qt::PreventContextMenu);

    QMenu* toolsMenu = menu->addMenu("Tools");

    QAction* settingsAction = new QAction(QIcon(":/icons/setting_tools.png"), "Settings", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsActionTriggered);

    toolsMenu->addActions(QList<QAction*>() << settingsAction);

    QDockWidget* friendDock = new QDockWidget(this);
    friendDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    friendDock->setTitleBarWidget(new QWidget(friendDock));
    friendDock->setContextMenuPolicy(Qt::PreventContextMenu);
    addDockWidget(Qt::LeftDockWidgetArea, friendDock);

    QWidget* friendDockWidget = new QWidget(friendDock);
    QVBoxLayout* layout = new QVBoxLayout(friendDockWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    ourUserItem = new OurUserItemWidget(this);
    friendsWidget = new FriendsWidget(friendDockWidget);

    layout->addWidget(ourUserItem);
    layout->addWidget(friendsWidget);

    friendDock->setWidget(friendDockWidget);

    PagesWidget* pages = new PagesWidget(this);
    connect(friendsWidget, &FriendsWidget::friendAdded, pages, &PagesWidget::addPage);
    connect(friendsWidget, &FriendsWidget::friendSelectionChanged, pages, &PagesWidget::activatePage);
    connect(friendsWidget, &FriendsWidget::friendStatusChanged, pages, &PagesWidget::statusChanged);

    //FIXME: start core in a separate function
    //all connections to `core` should be done after its creation because it registers some types
    core = new Core();

    coreThread = new QThread(this);
    core->moveToThread(coreThread);
    connect(coreThread, &QThread::started, core, &Core::start);

    connect(core, &Core::connected, this, &MainWindow::onConnected);
    connect(core, &Core::disconnected, this, &MainWindow::onDisconnected);
    connect(core, &Core::friendRequestRecieved, this, &MainWindow::onFriendRequestRecieved);
    connect(core, &Core::friendStatusChanged, this, &MainWindow::onFriendStatusChanged);
    connect(core, &Core::userIdGenerated, ourUserItem, &OurUserItemWidget::setUserId);
    connect(core, &Core::friendAdded, friendsWidget, &FriendsWidget::addFriend);
    connect(core, &Core::friendMessageRecieved, pages, &PagesWidget::messageReceived);
    connect(core, &Core::friendUsernameChanged, friendsWidget, &FriendsWidget::setUsername);
    connect(core, &Core::friendUsernameChanged, pages, &PagesWidget::usernameChanged);
    connect(core, &Core::friendRemoved, friendsWidget, &FriendsWidget::removeFriend);
    connect(core, &Core::friendRemoved, pages, &PagesWidget::removePage);
    connect(core, &Core::failedToRemoveFriend, this, &MainWindow::onFailedToRemoveFriend);
    connect(core, &Core::failedToAddFriend, this, &MainWindow::onFailedToAddFriend);
    connect(core, &Core::failedToSendMessage, pages, &PagesWidget::failedToSendMessage);

    coreThread->start(/*QThread::IdlePriority*/);

    connect(this, &MainWindow::friendRequestAccepted, core, &Core::acceptFirendRequest);

    connect(ourUserItem, &OurUserItemWidget::usernameChanged, core, &Core::setUsername);
    connect(core, &Core::usernameSet, ourUserItem, &OurUserItemWidget::setUsername);

    connect(pages, &PagesWidget::messageSent, core, &Core::sendMessage);

    connect(friendsWidget, &FriendsWidget::friendRequested, core, &Core::requestFriendship);
    connect(friendsWidget, &FriendsWidget::friendRemoved, core, &Core::removeFriend);

    setCentralWidget(pages);
}

MainWindow::~MainWindow()
{
    coreThread->quit();
    coreThread->wait();
    delete core;
}

void MainWindow::onFriendRequestRecieved(const QString& userId, const QString& message)
{
    FriendRequestDialog dialog(this, userId, message);

    if (dialog.exec() == QDialog::Accepted) {
        emit friendRequestAccepted(userId);
    }
}

void MainWindow::onFriendStatusChanged(int friendId, Core::FriendStatus status)
{
    switch (status) {
        case Core::FriendStatus::NotFound:
            qDebug() << "status:" << "no such friend found" << friendId;
            friendsWidget->setStatus(friendId, Status::Offline);
            break;
        case Core::FriendStatus::Added:
            qDebug() << "status:" << "friend was added" << friendId;
            friendsWidget->setStatus(friendId, Status::Offline);
            break;
        case Core::FriendStatus::RequestSent:
            qDebug() << "status:" << "friend request was sent" << friendId;
            friendsWidget->setStatus(friendId, Status::Offline);
            break;
        case Core::FriendStatus::Confirmed:
            qDebug() << "status:" << "friend is confirmed" << friendId;
            friendsWidget->setStatus(friendId, Status::Offline);
            break;
        case Core::FriendStatus::Online:
            qDebug() << "status:" << "friend is online" << friendId;
            friendsWidget->setStatus(friendId, Status::Online);
            break;
    }
}

void MainWindow::onConnected()
{
    ourUserItem->setStatus(Status::Online);
}

void MainWindow::onDisconnected()
{
    ourUserItem->setStatus(Status::Offline);
}

void MainWindow::onFailedToRemoveFriend(int friendId)
{
    QMessageBox critical(this);
    critical.setText(QString("Couldn't remove friend \"%1\"").arg(friendsWidget->getUsername(friendId)));
    critical.setIcon(QMessageBox::Critical);
    critical.exec();
}

void MainWindow::onFailedToAddFriend(const QString& userId)
{
    QMessageBox critical(this);
    critical.setText(QString("Couldn't add friend with User ID\n\"%1\"").arg(userId));
    critical.setIcon(QMessageBox::Critical);
    critical.exec();
}

void MainWindow::onSettingsActionTriggered()
{
    Settings::getInstance().executeSettingsDialog(this);
}
