/*
    Copyright (C) 2013 by Martin Kröll <technikschlumpf@web.de>

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

#include "friendrequestwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QWidgetAction>
#include <QMenu>

#include "elidelabel.hpp"

FriendRequestWidget::FriendRequestWidget(QWidget *parent) :
    QWidget(parent),
    it(requests)
{
    button = new QToolButton(this);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    menu = new QMenu( this);
    QWidgetAction *action = new QWidgetAction(this);
    actionDefaultWidget = new QWidget(this);
    QVBoxLayout *menulayout = new QVBoxLayout(actionDefaultWidget);
    menulayout->setContentsMargins(2,2,2,2);
    menulayout->setSpacing(2);
    action->setDefaultWidget(actionDefaultWidget);
    menu->addAction(action);

    clientId = new ElideLabel(actionDefaultWidget);
    clientId->setTextElide(true);
    clientId->setTextElideMode(Qt::ElideRight);

    message = new QLabel(actionDefaultWidget);
    message->setWordWrap(true);

    accaptButton = new QPushButton(actionDefaultWidget);
    accaptButton->setIcon(QIcon("://icons/accept.png"));

    rejectButton = new QPushButton(actionDefaultWidget);
    rejectButton->setIcon(QIcon("://icons/cancel.png"));

    connect(accaptButton, &QPushButton::clicked, this, &FriendRequestWidget::acceptUser);
    connect(rejectButton, &QPushButton::clicked, this, &FriendRequestWidget::rejectUser);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(accaptButton);
    buttonLayout->addWidget(rejectButton);
    buttonLayout->setContentsMargins(0,0,0,0);

    menulayout->addWidget(clientId);
    menulayout->addWidget(message);
    menulayout->addLayout(buttonLayout);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(button);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    updateGui();
}

void FriendRequestWidget::addFriendRequest(const QString &userId, const QString &msg)
{
    requests.insert(userId, msg);
    updateGui();
}

void FriendRequestWidget::acceptUser()
{
    emit userAccepted(it.key());
    it.remove();
    button->menu()->close();
    updateGui();
}

void FriendRequestWidget::rejectUser()
{
    it.remove();
    button->menu()->close();
    updateGui();
}

void FriendRequestWidget::updateGui()
{
    button->setText(tr("%n friend request(s)","", requests.count()));
    if(requests.count() > 0) {
        button->setMenu(menu);
        button->setVisible(true);
        it.toFront();
        it.next();

        clientId->setText(tr("Client ID: %1").arg(it.key()));
        clientId->setToolTip(it.key());
        message->setText(QString("\"<i>%1</i>\"").arg(it.value()));
    }
    else {
        button->setMenu(NULL);
        button->setVisible(false);
        clientId->clear();
        message->clear();
    }

    // Recalculate popup size
    if (button->menu()) {
        actionDefaultWidget->setFixedWidth(button->sizeHint().width());
        actionDefaultWidget->setFixedHeight(message->sizeHint().height()+clientId->sizeHint().height()+accaptButton->sizeHint().height()+4+2*2);
        button->menu()->setFixedWidth(button->sizeHint().width());
        button->menu()->setFixedHeight(message->sizeHint().height()+clientId->sizeHint().height()+accaptButton->sizeHint().height()+4+2*2);
    }
}
