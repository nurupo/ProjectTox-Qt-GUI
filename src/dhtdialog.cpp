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

#include "dhtdialog.hpp"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

DhtDialog::DhtDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("DHT Settings");

    QGroupBox* group = new QGroupBox("DHT Settings", this);

    QLabel* userIdLabel = new QLabel("User Id:", group);
    userIdEdit = new QLineEdit(group);

    QLabel* addressLabel = new QLabel("IP:", group);
    addressEdit = new QLineEdit(group);

    QLabel* portLabel = new QLabel("Port:", group);
    portSpinBox = new QSpinBox(group);
    portSpinBox->setMaximum(65535);
    portSpinBox->setMinimum(1);
    portSpinBox->setValue(33445);
    portSpinBox->setMinimumWidth(64);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok , Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DhtDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DhtDialog::reject);

    QGridLayout* groupLayout = new QGridLayout(group);

    groupLayout->addWidget(userIdLabel,     0, 0, 1, 1);
    groupLayout->addWidget(userIdEdit,      0, 1, 1, 3);
    groupLayout->addWidget(addressLabel,    1, 0, 1, 1);
    groupLayout->addWidget(addressEdit,     1, 1, 1, 1);
    groupLayout->addWidget(portLabel,       1, 2, 1, 1);
    groupLayout->addWidget(portSpinBox,     1, 3, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(group);
    layout->addWidget(buttonBox);
}

QString DhtDialog::getUserId() const
{
    return userIdEdit->text();
}

QString DhtDialog::getIp() const
{
    return addressEdit->text();
}

int DhtDialog::getPort() const
{
    return portSpinBox->value();
}
