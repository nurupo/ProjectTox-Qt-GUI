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
#include <QPropertyAnimation>
#include <QFrame>

#include "elidelabel.hpp"
#include "Settings/settings.hpp"

FriendRequestWidget::FriendRequestWidget(QWidget *parent) :
    QWidget(parent),
    it(requests)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    button = new QPushButton(this);
    button->setCheckable(true);
    button->setProperty("class", "frqButton"); // for CSS styling

    detailsWidget = new QFrame(this);
    detailsWidget->setFrameShape(QFrame::StyledPanel);
    detailsWidget->setProperty("class", "frqDetails"); // for CSS styling

    clientId = new ElideLabel(detailsWidget);
    clientId->setTextElide(true);
    clientId->setTextElideMode(Qt::ElideRight);
    clientId->setFixedWidth(100);

    message = new QLabel(detailsWidget);
    message->setWordWrap(true);

    acceptButton = new QToolButton(detailsWidget);
    acceptButton->setIcon(QIcon("://icons/accept.png"));
    acceptButton->setToolTip(tr("Accept request"));
    acceptButton->setAutoRaise(true);

    rejectButton = new QToolButton(detailsWidget);
    rejectButton->setIcon(QIcon("://icons/cancel.png"));
    rejectButton->setToolTip(tr("Reject request"));
    rejectButton->setAutoRaise(true);

    nextButton = new QToolButton(detailsWidget);
    nextButton->setIcon(QIcon("://icons/resultset_next.png"));
    nextButton->setToolTip(tr("Next request"));
    nextButton->setAutoRaise(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(rejectButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->setContentsMargins(0,0,0,0);

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setContentsMargins(2,2,2,2);
    detailsLayout->setSpacing(2);
    detailsLayout->addWidget(clientId);
    detailsLayout->addWidget(message);
    detailsLayout->addLayout(buttonLayout);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(button);
    layout->addWidget(detailsWidget);
    layout->setContentsMargins(4,4,4,4);
    layout->setSpacing(0);

    connect(button,       &QPushButton::toggled, this, &FriendRequestWidget::showDetails);
    connect(acceptButton, &QPushButton::clicked, this, &FriendRequestWidget::acceptUser);
    connect(rejectButton, &QPushButton::clicked, this, &FriendRequestWidget::rejectUser);
    connect(nextButton,   &QPushButton::clicked, this, &FriendRequestWidget::nextRequest);

    animation = new QPropertyAnimation(detailsWidget, "maximumHeight");
    animation->setDuration(200);
    animation->setLoopCount(1);

    if(Settings::getInstance().isAnimationEnabled()) {
        detailsWidget->setMaximumHeight(0);
    } else {
        detailsWidget->hide();
    }

    updateGui();
}

void FriendRequestWidget::addFriendRequest(const QString &userId, const QString &msg)
{
    requests.insert(userId, msg);
    goFirst();

    updateGui();
}

void FriendRequestWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    clientId->setMaximumWidth(detailsWidget->width());
}

void FriendRequestWidget::acceptUser()
{
    emit userAccepted(it.key());
    it.remove();
    goFirst();

    updateGui();
}

void FriendRequestWidget::rejectUser()
{
    it.remove();
    goFirst();

    updateGui();
}

void FriendRequestWidget::showDetails(bool show)
{
    if(show) {
        goFirst();

        detailsWidget->show();
        if(Settings::getInstance().isAnimationEnabled()) {
            animation->setKeyValueAt(0, 0);
            animation->setKeyValueAt(1, detailsWidget->sizeHint().height());
            animation->start();
        } else {
            detailsWidget->setMaximumHeight(detailsWidget->sizeHint().height());
        }
    }
    else {
        if(Settings::getInstance().isAnimationEnabled()) {
            animation->setKeyValueAt(0, detailsWidget->sizeHint().height());
            animation->setKeyValueAt(1, 0);
            animation->start();
        } else {
            detailsWidget->hide();
        }
    }
}

void FriendRequestWidget::nextRequest()
{
    if(it.hasNext()) {
        it.next();
    } else {
        goFirst();
    }
    updateGui();
}

void FriendRequestWidget::updateGui()
{
    button->setText(tr("%n friend request(s)","", requests.count()));
    if(requests.count() > 0) {
        setVisible(true);
        clientId->setText(tr("Client ID: %1").arg(it.key()));
        clientId->setToolTip(it.key());
        message->setText(QString("<i>\"%1\"</i>").arg(it.value()));

        // Hide next button
        if(requests.count() == 1) {
            nextButton->hide();
        } else {
            nextButton->show();
        }
    }
    else {
        button->setChecked(false);
        setVisible(false);
        clientId->clear();
        message->clear();
    }
}

/*! Go to first friend request */
void FriendRequestWidget::goFirst()
{
    it.toFront();
    if(it.hasNext()) {
        it.next();
    }
}
