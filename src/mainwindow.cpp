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

#include "aboutdialog.hpp"
#include "appinfo.hpp"
#include "friendrequestdialog.hpp"
#include "pageswidget.hpp"
#include "Settings/settings.hpp"
#include "closeapplicationdialog.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>
#include <QFontDatabase>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    const int screenWidth = QApplication::desktop()->width();
    const int screenHeight = QApplication::desktop()->height();
    const int appWidth = 500;
    const int appHeight = 300;

    setGeometry((screenWidth - appWidth) / 2, (screenHeight - appHeight) / 2, appWidth, appHeight);

    setObjectName("mainwindow");
    setWindowTitle("developers' test version, not for public use");
    setWindowIcon(QIcon(":/icons/icon64.png"));
    setContextMenuPolicy(Qt::PreventContextMenu);

    // install font
    int id = QFontDatabase::addApplicationFont("://DejaVuSans.ttf");
    qDebug() << QFontDatabase::applicationFontFamilies(id); ///////////////////////////////////////////////////////// TODO

    QDockWidget* friendDock = new QDockWidget(this);
    friendDock->setObjectName("FriendDock");
    friendDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    friendDock->setTitleBarWidget(new QWidget(friendDock));
    friendDock->setContextMenuPolicy(Qt::PreventContextMenu);
    addDockWidget(Qt::LeftDockWidgetArea, friendDock);

    QWidget* friendDockWidget = new QWidget(friendDock);
    QVBoxLayout* layout = new QVBoxLayout(friendDockWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
    friendDock->setWidget(friendDockWidget);

    ourUserItem = new OurUserItemWidget(this);
    friendsWidget = new FriendsWidget(friendDockWidget);

    // Create toolbar
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(16, 16));

    QToolButton *addFriendButton = new QToolButton(toolBar);
    addFriendButton->setIcon(QIcon("://icons/user_add.png"));
    addFriendButton->setToolTip(tr("Add friend"));
    connect(addFriendButton, &QToolButton::clicked, friendsWidget, &FriendsWidget::onAddFriendButtonClicked);

    QWidget *spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QToolButton *menuButton = new QToolButton(toolBar);
    menuButton->setIcon(QIcon("://icons/cog.png"));
    menuButton->setToolTip(tr("Mainmenu"));
    menuButton->setPopupMode(QToolButton::InstantPopup);
    QMenu *mainmenu = new QMenu(menuButton);
    mainmenu->addAction(QIcon(":/icons/setting_tools.png"), tr("Settings"), this, SLOT(onSettingsActionTriggered()));
    mainmenu->addSeparator();
    mainmenu->addAction(tr("About %1").arg(AppInfo::name), this, SLOT(onAboutAppActionTriggered()));
    mainmenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));
    mainmenu->addSeparator();
    mainmenu->addAction(tr("Quit"), this, SLOT(onQuitApplicationTriggered()), QKeySequence::Quit);
    menuButton->setMenu(mainmenu);

    toolBar->addWidget(addFriendButton);
    toolBar->addWidget(spacer);
    toolBar->addWidget(menuButton);
    // Create toolbar end

    layout->addWidget(ourUserItem);
    layout->addWidget(friendsWidget);
    layout->addWidget(toolBar);

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

    qRegisterMetaType<Status>("Status");

    connect(core, &Core::connected, this, &MainWindow::onConnected);
    connect(core, &Core::disconnected, this, &MainWindow::onDisconnected);
    connect(core, &Core::friendRequestRecieved, this, &MainWindow::onFriendRequestRecieved);
    connect(core, SIGNAL(friendStatusChanged(int, Status)), friendsWidget, SLOT(setStatus(int, Status)));
    connect(core, &Core::friendAddressGenerated, ourUserItem, &OurUserItemWidget::setFriendAddress);
    connect(core, &Core::friendAdded, friendsWidget, &FriendsWidget::addFriend);
    connect(core, &Core::friendMessageRecieved, pages, &PagesWidget::messageReceived);
    connect(core, &Core::friendUsernameChanged, friendsWidget, &FriendsWidget::setUsername);
    connect(core, &Core::friendUsernameChanged, pages, &PagesWidget::usernameChanged);
    connect(core, &Core::friendRemoved, friendsWidget, &FriendsWidget::removeFriend);
    connect(core, &Core::friendRemoved, pages, &PagesWidget::removePage);
    connect(core, &Core::failedToRemoveFriend, this, &MainWindow::onFailedToRemoveFriend);
    connect(core, &Core::failedToAddFriend, this, &MainWindow::onFailedToAddFriend);
    connect(core, &Core::messageSentResult, pages, &PagesWidget::messageSentResult);

    coreThread->start(/*QThread::IdlePriority*/);

    connect(this, &MainWindow::friendRequestAccepted, core, &Core::acceptFriendRequest);

    connect(ourUserItem, &OurUserItemWidget::usernameChanged, core, &Core::setUsername);
    connect(core, &Core::usernameSet, ourUserItem, &OurUserItemWidget::setUsername);

    connect(ourUserItem, &OurUserItemWidget::statusSelected, core, &Core::setStatus);

    connect(pages, &PagesWidget::sendMessage, core, &Core::sendMessage);

    connect(friendsWidget, &FriendsWidget::friendRequested, core, &Core::requestFriendship);
    connect(friendsWidget, &FriendsWidget::friendRemoved, core, &Core::removeFriend);

    setCentralWidget(pages);

    Settings::getInstance().loadWindow(this);
}

MainWindow::~MainWindow()
{
    coreThread->quit();
    coreThread->wait();
    delete core;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::getInstance().saveWindow(this);
    QMainWindow::closeEvent(event);
}

void MainWindow::onFriendRequestRecieved(const QString& userId, const QString& message)
{
    FriendRequestDialog dialog(this, userId, message);

    if (dialog.exec() == QDialog::Accepted) {
        emit friendRequestAccepted(userId);
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

void MainWindow::onAboutAppActionTriggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onQuitApplicationTriggered()
{
    CloseApplicationDialog dialog(this);
    dialog.exec();
}
