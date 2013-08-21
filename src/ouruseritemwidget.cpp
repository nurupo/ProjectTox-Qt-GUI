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

    usernameLabel = new ElideLabel(usernameStackedWidget);
    usernameLabel->setMinimumWidth(10);
    usernameLabel->setTextElide(true);
    usernameLabel->setTextElideMode(Qt::ElideRight);
    usernameLabel->setText(Settings::getInstance().getUsername());

    //FIXME: check if there is a way to replace these containers to fix the label alignment
    QWidget* usernameLabelContainer = new QWidget(this);
    QVBoxLayout* usernameLabelContainerLayout = new QVBoxLayout(usernameLabelContainer);
    usernameLabelContainerLayout->setMargin(0);
    usernameLabelContainerLayout->addWidget(usernameLabel, 0, Qt::AlignVCenter);

    usernameEdit = new RenameEditWidget(usernameStackedWidget, QSize(10, 10));
    usernameEdit->setMinimumWidth(10);
    connect(usernameEdit, &QLineEdit::editingFinished,      this, &OurUserItemWidget::onUsernameChangeSubmited);
    connect(usernameEdit, &RenameEditWidget::escPressed,    this, &OurUserItemWidget::onUsernameChangeCancelled);

    usernameStackedWidget->addWidget(usernameLabelContainer);
    usernameStackedWidget->addWidget(usernameEdit);
    usernameStackedWidget->setCurrentWidget(usernameLabelContainer);

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

void OurUserItemWidget::onUsernameChangeSubmited()
{
    QString newUsername = usernameEdit->text();
    //restore old username, usernameLabel still contains it
    usernameEdit->setText(usernameLabel->text());
    usernameEdit->clearFocus();
    usernameStackedWidget->setCurrentWidget(usernameLabelContainer);
    emit usernameChanged(newUsername);
}

void OurUserItemWidget::onUsernameChangeCancelled()
{
    //restore old username, usernameLabel still contains it
    usernameEdit->setText(usernameLabel->text());
    usernameEdit->clearFocus();
    usernameStackedWidget->setCurrentWidget(usernameLabelContainer);
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
