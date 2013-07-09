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
#include "friendswidget.hpp"
#include "pageswidget.hpp"
#include "ouruseritemwidget.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    const int screenWidth = QApplication::desktop()->width();
    const int screenHeight = QApplication::desktop()->height();
    const int appWidth = 500;
    const int appHeight = 300;

    setGeometry((screenWidth - appWidth) / 2, (screenHeight - appHeight) / 2, appWidth, appHeight);

    setWindowTitle("TOX GUI, test version");

    QDockWidget* friendDock = new QDockWidget(this);
    friendDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    friendDock->setTitleBarWidget(new QWidget(friendDock));
    friendDock->setContextMenuPolicy(Qt::PreventContextMenu);
    addDockWidget(Qt::LeftDockWidgetArea, friendDock);

    QWidget* friendDockWidget = new QWidget(friendDock);
    QVBoxLayout* layout = new QVBoxLayout(friendDockWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    OurUserItemWidget* ourUserItem = new OurUserItemWidget(friendDockWidget);
    FriendsWidget* friendsWidget = new FriendsWidget(friendDockWidget);

    layout->addWidget(ourUserItem);
    layout->addWidget(friendsWidget);

    friendDock->setWidget(friendDockWidget);

    PagesWidget* pages = new PagesWidget(this);
    connect(friendsWidget, &FriendsWidget::friendAdded, pages, &PagesWidget::addPage);
    connect(friendsWidget, &FriendsWidget::friendRemoved, pages, &PagesWidget::removePage);
    connect(friendsWidget, &FriendsWidget::friendSelectionChanged, pages, &PagesWidget::activatePage);
    connect(friendsWidget, &FriendsWidget::friendRenamed, pages, &PagesWidget::usernameChanged);
    connect(friendsWidget, &FriendsWidget::friendStatusChanged, pages, &PagesWidget::statusChanged);

    friendsWidget->addFriend("userid", "abcd");
    friendsWidget->addFriend("userid", "ABCD");
    friendsWidget->addFriend("abcd", "ablasche");
    friendsWidget->setStatus("abcd", Status::Online);
    friendsWidget->addFriend("qwerty", "adamm");
    friendsWidget->addFriend("nafjaewilfj", "Adaptee");
    friendsWidget->setStatus("nafjaewilfj", Status::Online);
    friendsWidget->addFriend("144214124", "aeffle");
    friendsWidget->addFriend("artthger", "ahf");


    setCentralWidget(pages);

    /*worker = new DhtWorker();
    worker->init(33449);

    workerThread = new QThread(this);
    worker->moveToThread(workerThread);
    connect(workerThread, &QThread::started, worker, &DhtWorker::run);
    workerThread->start();*/
}

MainWindow::~MainWindow()
{
}

void MainWindow::sendMessage()
{

}
