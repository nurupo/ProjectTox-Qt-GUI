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

#ifndef PAGESWIDGET_HPP
#define PAGESWIDGET_HPP

#include "chatpagewidget.hpp"

#include <QStackedWidget>

class PagesWidget : public QStackedWidget
{
    Q_OBJECT
public:
    PagesWidget(QWidget* parent);

private:
    ChatPageWidget* widget(const QString& userId) const;

private slots:
    void onMessageSent(const QString &message);

public slots:
    void addPage(const QString& userId, const QString& username);
    void activatePage(const QString& userId);
    void removePage(const QString& userId);
    void statusChanged(const QString& userId, Status status);
    void usernameChanged(const QString& userId, const QString& username);
    void messageReceived(const QString &userId, const QString &message);

signals:
    void messageSent(const QString &userId, const QString &message);

};

#endif // PAGESWIDGET_HPP
