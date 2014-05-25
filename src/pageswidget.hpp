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
    ChatPageWidget* widget(int friendId) const;

private slots:
    void onMessageSent(const QString& message);
    void onActionToSend(const QString& action);

public slots:
    void addPage(int friendId, const QString& username);
    void removePage(int friendId);
    void activatePage(int friendId);
    void statusChanged(int friendId, Status status);
    void onFriendusernameChanged(int friendId, const QString& username);
    void onOurUsernameChanged(const QString& username);
    void onFriendTypingChanged(int friendId, bool isTyping);
    void statusMessageChanged(int friendId, const QString& statusMessage);
    void messageReceived(int friendId, const QString& message);
    void actionReceived(int friendId, const QString& message);

    void messageSentResult(int friendId, const QString& message, int messageId);
    void actionResult(int friendId, const QString &action, int success);

signals:
    void sendMessage(int friendId, const QString& message);
    void sendAction(int friendId, const QString& action);

};

#endif // PAGESWIDGET_HPP
