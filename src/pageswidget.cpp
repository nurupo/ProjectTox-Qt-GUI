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

#include "chatpagewidget.hpp"
#include "pageswidget.hpp"

#include <QDebug>

PagesWidget::PagesWidget(QWidget* parent) :
    QStackedWidget(parent)
{
    addWidget(new QWidget(this));
}

// if preformance would be critical, replace with QHash<QString userId, ChatPageWidget*>
ChatPageWidget* PagesWidget::widget(const QString& userId) const
{
    for (int i = 0; i < count(); i ++) {
        ChatPageWidget* pageWidget = dynamic_cast<ChatPageWidget*>(QStackedWidget::widget(i));
        if (pageWidget != nullptr && pageWidget->getUserId() == userId) {
            return pageWidget;
        }
    }
    return nullptr;
}

void PagesWidget::addPage(const QString& userId, const QString& username)
{
    ChatPageWidget* pageWidget = new ChatPageWidget(userId, this);
    pageWidget->setUsername(username);
    addWidget(pageWidget);
    qDebug() << "page" << userId << "added" << count();
}

void PagesWidget::activatePage(const QString& userId)
{
    setCurrentWidget(widget(userId));
}

void PagesWidget::removePage(const QString& userId)
{
    ChatPageWidget* pageWidget = widget(userId);
    removeWidget(pageWidget);
    delete pageWidget;
    qDebug() << "page" << userId << "removed" << count();
}

void PagesWidget::usernameChanged(const QString& userId, const QString& username)
{
    widget(userId)->setUsername(username);
}

void PagesWidget::statusChanged(const QString& userId, Status status)
{
    widget(userId)->setStatus(status);
}
