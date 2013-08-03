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

#include "dhtserverdialog.hpp"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>

DhtServerDialog::DhtServerDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("DHT Settings"));

    QGroupBox* group = new QGroupBox(tr("DHT Settings"), this);

    QLabel* nameLabel = new QLabel(tr("Name:"), group);
    nameEdit = new QLineEdit(group);

    QLabel* userIdLabel = new QLabel(tr("User ID:"), group);
    userIdEdit = new QLineEdit(group);

    QLabel* addressLabel = new QLabel(tr("IP:"), group);
    addressEdit = new QLineEdit(group);

    QLabel* portLabel = new QLabel(tr("Port:"), group);
    portSpinBox = new QSpinBox(group);
    portSpinBox->setMaximum(65535);
    portSpinBox->setMinimum(1);
    portSpinBox->setValue(33445);
    portSpinBox->setMinimumWidth(64);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok , Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DhtServerDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DhtServerDialog::reject);

    QGridLayout* groupLayout = new QGridLayout(group);

    groupLayout->addWidget(nameLabel,       0, 0, 1, 1);
    groupLayout->addWidget(nameEdit,        0, 1, 1, 3);
    groupLayout->addWidget(userIdLabel,     1, 0, 1, 1);
    groupLayout->addWidget(userIdEdit,      1, 1, 1, 3);
    groupLayout->addWidget(addressLabel,    2, 0, 1, 1);
    groupLayout->addWidget(addressEdit,     2, 1, 1, 1);
    groupLayout->addWidget(portLabel,       2, 2, 1, 1);
    groupLayout->addWidget(portSpinBox,     2, 3, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(group);
    layout->addWidget(buttonBox);
}

void DhtServerDialog::setServerInformation(const Settings::DhtServer& server)
{
    nameEdit->setText(server.name);
    userIdEdit->setText(server.userId);
    addressEdit->setText(server.address);
    portSpinBox->setValue(server.port);

    userIdEdit->setCursorPosition(0);
}

Settings::DhtServer DhtServerDialog::getServerInformation() const
{
    Settings::DhtServer server;

    server.name = nameEdit->text();
    server.userId = userIdEdit->text();
    server.address = addressEdit->text();
    server.port = portSpinBox->value();

    return server;
}

void DhtServerDialog::accept()
{
    const QRegularExpression hexRegExp("^[A-Fa-f0-9]+$");

    if (nameEdit->text().length() == 0 || userIdEdit->text().length() == 0 || addressEdit->text().length() == 0) {
        QMessageBox warning(this);
        warning.setText(tr("Please fill all the fields in."));
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else if (userIdEdit->text().length() != 64 || !userIdEdit->text().contains(hexRegExp)) {
        QMessageBox warning(this);
        warning.setText(tr("Please enter a valid User ID."));
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else {
        QDialog::accept();
    }
}
