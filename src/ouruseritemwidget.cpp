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

#include "ouruseritemwidget.hpp"
#include "Settings/settings.hpp"
#include "closeapplicationdialog.hpp"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>

OurUserItemWidget::OurUserItemWidget(QWidget* parent) :
    QWidget(parent)
{
    statusButton = createToolButton(QIcon(StatusHelper::getInfo(Status::Offline).iconPath), QSize(24, 24), "Change Status");
    statusButton->setPopupMode(QToolButton::InstantPopup);

    QToolButton* copyFriendAddressButton = createToolButton(QIcon(":/icons/page_copy.png"), QSize(16, 16), "Copy my Friend Address");
    connect(copyFriendAddressButton, &QToolButton::clicked, this, &OurUserItemWidget::onCopyFriendAddressButtonClicked);

    QMenu* statusMenu = new QMenu(statusButton);
    QList<QAction*> statusActions;
    for (int i = 0; i <= StatusHelper::MAX_STATUS; i ++) {
        StatusHelper::Info statusInfo = StatusHelper::getInfo(i);
        QAction* statusAction = new QAction(QIcon(statusInfo.iconPath), statusInfo.name, statusMenu);
        statusAction->setData(i);
        connect(statusAction, &QAction::triggered, this, &OurUserItemWidget::onStatusActionTriggered);
        statusActions << statusAction;
    }
    statusMenu->addActions(QList<QAction*>() << statusActions);
    statusButton->setMenu(statusMenu);

    usernameWidget = new EditableLabelWidget(this);
    usernameWidget->setText(Settings::getInstance().getUsername());
    usernameWidget->label->setTextElide(true);
    usernameWidget->label->setTextElideMode(Qt::ElideRight);
    usernameWidget->label->setShowToolTipOnElide(true);
    usernameWidget->setMinimumWidth(10);
    connect(usernameWidget, &EditableLabelWidget::textChanged, this, &OurUserItemWidget::onUsernameChanged);

    statusMessageWidget = new EditableLabelWidget(this);
    statusMessageWidget->setText(Settings::getInstance().getStatusMessage());
    statusMessageWidget->label->setTextElide(true);
    statusMessageWidget->label->setTextElideMode(Qt::ElideRight);
    statusMessageWidget->label->setShowToolTipOnElide(true);
    statusMessageWidget->setMinimumWidth(10);
    QPalette palette;
    palette.setColor(QPalette::Foreground, Qt::gray);
    statusMessageWidget->label->setPalette(palette);
    connect(statusMessageWidget, &EditableLabelWidget::textChanged, this, &OurUserItemWidget::onStatusMessageChanged);

    QWidget* userInformationWidget = new QWidget(this);
    QVBoxLayout* userInformationLayout = new QVBoxLayout(userInformationWidget);
    userInformationLayout->setContentsMargins(0, 0, 0, 0);
    userInformationLayout->setSpacing(0);
    userInformationLayout->addWidget(usernameWidget, 0, Qt::AlignVCenter);
    userInformationLayout->addWidget(statusMessageWidget, 0, Qt::AlignVCenter);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 0);
    layout->setSpacing(2);
    layout->addWidget(statusButton, 0, Qt::AlignVCenter);
    layout->addWidget(userInformationWidget);
    layout->addWidget(copyFriendAddressButton, 0, Qt::AlignRight | Qt::AlignVCenter);

    setFocusPolicy(Qt::ClickFocus);
}

QToolButton* OurUserItemWidget::createToolButton(const QIcon& icon, const QSize iconSize, const QString& toolTip)
{
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setIcon(icon);
    toolButton->setIconSize(iconSize);
    toolButton->setToolTip(toolTip);
    toolButton->setAutoRaise(true);

    return toolButton;
}

void OurUserItemWidget::onUsernameChanged(const QString& newUsername, const QString& oldUsername)
{
    // restore old username until Core tells us to set it
    usernameWidget->setText(oldUsername);

    // ask Core to change it
    emit usernameChanged(newUsername);
}

void OurUserItemWidget::setUsername(const QString& username)
{
    usernameWidget->setText(username);
    Settings::getInstance().setUsername(username);
}

void OurUserItemWidget::onStatusMessageChanged(const QString& newStatusMessage, const QString& oldStatusMessage)
{
    // restore old status message until Core tells us to set it
    statusMessageWidget->setText(oldStatusMessage);

    // ask Core to change it
    emit statusMessageChanged(newStatusMessage);
}

void OurUserItemWidget::setStatusMessage(const QString &statusMessage)
{
    statusMessageWidget->setText(statusMessage);
    Settings::getInstance().setStatusMessage(statusMessage);
}

void OurUserItemWidget::onStatusActionTriggered()
{
    QAction* statusAction = static_cast<QAction*>(sender());
    Status selectedStatus = static_cast<Status>(statusAction->data().toInt());

    //close application on going offline, since core doesn't implement going offline functionality
    //TODO: when core implements it, go offline instead of closing the application
    if (selectedStatus == Status::Offline) {
        CloseApplicationDialog dialog(this);
        dialog.exec();
    } else {
        statusButton->setIcon(QIcon(StatusHelper::getInfo(selectedStatus).iconPath));
        emit statusSelected(selectedStatus);
    }
}

void OurUserItemWidget::onCopyFriendAddressButtonClicked()
{
    QGuiApplication::clipboard()->setText(friendAddress);
}

void OurUserItemWidget::setFriendAddress(const QString &friendAddress)
{
    this->friendAddress = friendAddress;
}

void OurUserItemWidget::setStatus(Status status)
{
    statusButton->setIcon(QIcon(StatusHelper::getInfo(status).iconPath));
}
