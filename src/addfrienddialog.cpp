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
    setWindowTitle(tr("Add Friend"));

    QGroupBox* addFriendGroup = new QGroupBox(tr("Add friend"), this);

    QLabel* userIdLabel = new QLabel(tr("User ID:"), addFriendGroup);
    userIdEdit = new QLineEdit(addFriendGroup);

    QLabel* messageLabel = new QLabel(tr("Message:"), addFriendGroup);
    messageEdit = new QPlainTextEdit(addFriendGroup);
    messageEdit->setPlainText(tr("Hello, please add me to your friend list."));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddFriendDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddFriendDialog::reject);

    QGridLayout* groupLayout = new QGridLayout(addFriendGroup);

    groupLayout->addWidget(userIdLabel,     0, 0, 1, 1);
    groupLayout->addWidget(userIdEdit,      0, 1, 1, 1);
    groupLayout->addWidget(messageLabel,    1, 0, 1, 1, Qt::AlignTop);
    groupLayout->addWidget(messageEdit,     1, 1, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(addFriendGroup);
    layout->addWidget(buttonBox);

    resize(300, 100);
}

QString AddFriendDialog::getUserId() const
{
    return userIdEdit->text();
}

QString AddFriendDialog::getMessage() const
{
    return messageEdit->toPlainText();
}

void AddFriendDialog::accept()
{
    const QRegularExpression regExp("^[A-Fa-f0-9]+$");

    if (userIdEdit->text().length() == 0 || messageEdit->toPlainText().length() == 0) {
        QMessageBox warning(this);
        warning.setText(tr("Please fill all the fields in."));
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else if (userIdEdit->text().length() != 64 || !userIdEdit->text().contains(regExp)) {
        QMessageBox warning(this);
        warning.setText(tr("UserID must be 64 HEX chars."));
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else {
        QDialog::accept();
    }
}
