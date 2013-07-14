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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "core.hpp"
#include "friendswidget.hpp"

#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QTextBrowser>
#include <QThread>


class OurUserItemWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static OurUserItemWidget* ourUserItem;

private:
    Core* core;
    QThread* coreThread;
    FriendsWidget* friendsWidget;

private slots:
    void sendMessage();
    void onFriendRequestRecieved(const QString &userId, const QString &message);
    void onFriendStatusChanged(const QString &userId, Core::FriendStatus status);

signals:
    void friendRequestAccepted(const QString &userId);

};

#endif // MAINWINDOW_HPP
