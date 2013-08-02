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

    QToolButton* renameUsernameButton = createToolButton(QIcon(":/icons/textfield_rename.png"), QSize(16, 16), "Change Username");
    QToolButton* copyUserIdButton = createToolButton(QIcon(":/icons/page_copy.png"), QSize(16, 16), "Copy User ID");
    connect(renameUsernameButton, &QToolButton::clicked, this, &OurUserItemWidget::onRenameUsernameButtonClicked);
    connect(copyUserIdButton, &QToolButton::clicked, this, &OurUserItemWidget::onCopyUserIdButtonClicked);

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

    usernameStackedWidget = new QStackedWidget(this);

    usernameLabel = new QLabel(Settings::getInstance().getUsername(), usernameStackedWidget);

    usernameEdit = new RenameEditWidget(usernameStackedWidget, QSize(10, 10));
    connect(usernameEdit, &QLineEdit::editingFinished,      this, &OurUserItemWidget::onUsernameChange);
    connect(usernameEdit, &RenameEditWidget::escPressed,    this, &OurUserItemWidget::onUsernameChange);

    usernameStackedWidget->addWidget(usernameLabel);
    usernameStackedWidget->addWidget(usernameEdit);
    usernameStackedWidget->setCurrentWidget(usernameLabel);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 0);
    layout->setSpacing(2);

    layout->addWidget(statusButton, 0, Qt::AlignVCenter);
    layout->addWidget(usernameStackedWidget, 10, Qt::AlignVCenter);
    layout->addWidget(renameUsernameButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(copyUserIdButton, 0, Qt::AlignRight | Qt::AlignVCenter);
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

void OurUserItemWidget::onUsernameChange()
{
    QString newUsername = usernameEdit->text();
    //restore old username, usernameLabel still contains it
    usernameEdit->setText(usernameLabel->text());
    usernameEdit->clearFocus();
    usernameStackedWidget->setCurrentWidget(usernameLabel);
    emit usernameChanged(newUsername);
}

void OurUserItemWidget::setUsername(const QString& username)
{
    usernameEdit->setText(username);
    usernameLabel->setText(username);
    Settings::getInstance().setUsername(username);
}

void OurUserItemWidget::onRenameUsernameButtonClicked()
{
    usernameEdit->setText(usernameLabel->text());
    usernameStackedWidget->setCurrentWidget(usernameEdit);
    if (usernameEdit->hasFocus()) {
        usernameEdit->clearFocus();
    } else {
        usernameEdit->setFocus();
    }
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
    }
}

void OurUserItemWidget::onCopyUserIdButtonClicked()
{
    QGuiApplication::clipboard()->setText(userId);
}

void OurUserItemWidget::setUserId(const QString &userId)
{
    this->userId = userId;
}

void OurUserItemWidget::setStatus(Status status)
{
    statusButton->setIcon(QIcon(StatusHelper::getInfo(status).iconPath));
}
