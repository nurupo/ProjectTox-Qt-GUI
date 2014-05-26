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

#include "addfrienddialog.hpp"
#include "customhinttextedit.hpp"

#include <tox/tox.h>

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>

AddFriendDialog::AddFriendDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Add Friend");

    QGroupBox* addFriendGroup = new QGroupBox("Add friend", this);

    QLabel* friendAddressLabel = new QLabel("Friend Address:", addFriendGroup);
    friendAddressEdit = new QLineEdit(addFriendGroup);

    QLabel* messageLabel = new QLabel("Message:", addFriendGroup);
    messageEdit = new QPlainTextEdit(addFriendGroup);
    messageEdit->setPlainText("Hello, please add me to your friend list.");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddFriendDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddFriendDialog::reject);

    QGridLayout* groupLayout = new QGridLayout(addFriendGroup);

    groupLayout->addWidget(friendAddressLabel,  0, 0, 1, 1);
    groupLayout->addWidget(friendAddressEdit,   0, 1, 1, 1);
    groupLayout->addWidget(messageLabel,        1, 0, 1, 1, Qt::AlignTop);
    groupLayout->addWidget(messageEdit,         1, 1, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(addFriendGroup);
    layout->addWidget(buttonBox);

    resize(324, 100);
}

QString AddFriendDialog::getFriendAddress() const
{
    return friendAddressEdit->text().trimmed();
}

QString AddFriendDialog::getMessage() const
{
    return messageEdit->toPlainText();
}

void AddFriendDialog::accept()
{
    const QRegularExpression hexRegExp("^[A-Fa-f0-9]+$");

    QString trimmedFriendAddress = friendAddressEdit->text().trimmed();

    if (trimmedFriendAddress.length() == 0 || messageEdit->toPlainText().length() == 0) {
        QMessageBox warning(this);
        warning.setText("Please fill all the fields in.");
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else if (trimmedFriendAddress.length() != (TOX_FRIEND_ADDRESS_SIZE * 2) || !friendAddressEdit->text().contains(hexRegExp)) {
        QMessageBox warning(this);
        warning.setText("Please enter a valid Friend address.");
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else {
        QDialog::accept();
    }
}
