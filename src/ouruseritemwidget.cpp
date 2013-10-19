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
    QToolButton* copyFriendAddressButton = createToolButton(QIcon(":/icons/page_copy.png"), QSize(16, 16), "Copy Friend Address");
    connect(renameUsernameButton, &QToolButton::clicked, this, &OurUserItemWidget::onRenameUsernameButtonClicked);
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

    usernameStackedWidget = new QStackedWidget(this);

    usernameLabel = new ElideLabel(usernameStackedWidget);
    usernameLabel->setMinimumWidth(10);
    usernameLabel->setTextElide(true);
    usernameLabel->setTextElideMode(Qt::ElideRight);
    usernameLabel->setText(Settings::getInstance().getUsername());

    statusMessageStackedWidget = new QStackedWidget(this);

    statusMessageLabel = new ElideLabel(statusMessageStackedWidget);
    statusMessageLabel->setMinimumWidth(10);
    statusMessageLabel->setTextElide(true);
    statusMessageLabel->setTextElideMode(Qt::ElideRight);
    statusMessageLabel->setText(Settings::getInstance().getStatusMessage());
    connect(statusMessageLabel, &ElideLabel::clicked, this, &OurUserItemWidget::onChangeStatusMessageButtonClicked);

    //Style statusMessageLabel
    const double statusMessageFontSizeRatio = 1.1;  //change this number to make the status message font size larger or smaller
    QFont font = usernameLabel->font();
    font.setPointSize(font.pointSize()/statusMessageFontSizeRatio);
    statusMessageLabel->setFont(font);
    statusMessageLabel->setStyleSheet("QLabel { color: grey }");    //make the status message color grey

    //FIXME: check if there is a way to replace these containers to fix the label alignment
    //Username UI elements
    usernameLabelContainer = new QWidget(this);
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
    usernameStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    //Status Message UI elements
    statusMessageLabelContainer = new QWidget(this);
    QVBoxLayout* statusMessageLabelContainerLayout = new QVBoxLayout(statusMessageLabelContainer);
    statusMessageLabelContainerLayout->setMargin(0);
    statusMessageLabelContainerLayout->addWidget(statusMessageLabel, 0, Qt::AlignVCenter);

    statusMessageEdit = new RenameEditWidget(statusMessageStackedWidget, QSize(10, 10));
    statusMessageEdit->setMinimumWidth(10);
    connect(statusMessageEdit, &QLineEdit::editingFinished, this, &OurUserItemWidget::onStatusMessageChangeSubmited);
    connect(statusMessageEdit, &RenameEditWidget::escPressed, this, &OurUserItemWidget::onStatusMessageChangeCancelled);

    statusMessageStackedWidget->addWidget(statusMessageLabelContainer);
    statusMessageStackedWidget->addWidget(statusMessageEdit);
    statusMessageStackedWidget->setCurrentWidget(statusMessageLabelContainer);
    statusMessageStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QWidget* userInformationWidget = new QWidget(this);
    QVBoxLayout* userInformationLayout = new QVBoxLayout(userInformationWidget);
    userInformationLayout->setContentsMargins(0,0,0,0);
    userInformationLayout->setSpacing(0);
    userInformationLayout->addWidget(usernameStackedWidget);
    userInformationLayout->addWidget(statusMessageStackedWidget);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 0);
    layout->setSpacing(2);
    layout->addWidget(statusButton, 0, Qt::AlignVCenter);
    layout->addWidget(userInformationWidget);
    layout->addWidget(renameUsernameButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(copyFriendAddressButton, 0, Qt::AlignRight | Qt::AlignVCenter);
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

void OurUserItemWidget::setStatusMessage(const QString &statusMessage)
{
    statusMessageEdit->setText(statusMessage);
    statusMessageLabel->setText(statusMessage);
    Settings::getInstance().setStatusMessage(statusMessage);
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
    } else {
        statusButton->setIcon(QIcon(StatusHelper::getInfo(selectedStatus).iconPath));
        emit statusSelected(selectedStatus);
    }
}

void OurUserItemWidget::onCopyFriendAddressButtonClicked()
{
    QGuiApplication::clipboard()->setText(friendAddress);
}

void OurUserItemWidget::onStatusMessageChangeSubmited()
{
    QString newStatusMessage = statusMessageEdit->text();
    //restore old statusMessage, statusMessageLabel still contains it
    statusMessageEdit->setText(statusMessageLabel->text());
    statusMessageEdit->clearFocus();
    statusMessageStackedWidget->setCurrentWidget(statusMessageLabelContainer);
    emit statusMessageChanged(newStatusMessage);
}

void OurUserItemWidget::onStatusMessageChangeCancelled()
{
    statusMessageEdit->setText(statusMessageLabel->text());
    statusMessageEdit->clearFocus();
    statusMessageStackedWidget->setCurrentWidget(statusMessageLabelContainer);
}

void OurUserItemWidget::onChangeStatusMessageButtonClicked()
{
    statusMessageEdit->setText(statusMessageLabel->text());
    statusMessageStackedWidget->setCurrentWidget(statusMessageEdit);
    if (statusMessageEdit->hasFocus()) {
        statusMessageEdit->clearFocus();
    } else {
        statusMessageEdit->setFocus();
    }
}

void OurUserItemWidget::setFriendAddress(const QString &friendAddress)
{
    this->friendAddress = friendAddress;
}

void OurUserItemWidget::setStatus(Status status)
{
    statusButton->setIcon(QIcon(StatusHelper::getInfo(status).iconPath));
}
