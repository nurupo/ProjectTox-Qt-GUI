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
#include "dhteditdialog.hpp"
#include "Settings/settings.hpp"
#include "closeapplicationdialog.hpp"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

DhtDialog::DhtDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Select DHT Server");

    QGroupBox* group = new QGroupBox("DHT Settings", this);

    serverComboBox = new QComboBox(this);

    serverModel = new QStandardItemModel(this);

    const Settings& settings = Settings::getInstance();

    const QList<Settings::DhtServer>& serverList = settings.getDhtServerList();
    modifiedServerList = serverList;
    for (const Settings::DhtServer& server : serverList) {
        serverModel->appendRow(new QStandardItem(server.name));
    }

    serverComboBox->setModel(serverModel);
    serverComboBox->setCurrentIndex(settings.getDhtServerId());

    QPushButton* addButton = new QPushButton("Add", this);
    QPushButton* editButton = new QPushButton("Edit", this);
    QPushButton* removeButton = new QPushButton("Remove", this);

    connect(addButton,    &QPushButton::clicked, this, &DhtDialog::addButtonClicked);
    connect(editButton,   &QPushButton::clicked, this, &DhtDialog::editButtonClicked);
    connect(removeButton, &QPushButton::clicked, this, &DhtDialog::removeButtonClicked);

    dontShowCheckBox = new QCheckBox("Don't show this dialog");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DhtDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DhtDialog::reject);

    QGridLayout* groupLayout = new QGridLayout(group);

    groupLayout->addWidget(serverComboBox,  0, 0, 1, 3);
    groupLayout->addWidget(addButton,       1, 0, 1, 1);
    groupLayout->addWidget(editButton,      1, 1, 1, 1);
    groupLayout->addWidget(removeButton,    1, 2, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(group);
    layout->addWidget(dontShowCheckBox);
    layout->addWidget(buttonBox);
}

void DhtDialog::accept()
{
    if (serverComboBox->currentIndex() == -1) {
        QMessageBox warning(this);
        warning.setText("You haven't selected any DHT server.");
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    } else {
        Settings& settings = Settings::getInstance();
        settings.setDhtServerList(modifiedServerList);
        settings.setDhtServerId(serverComboBox->currentIndex());
        settings.setDontShowDhtDialog(dontShowCheckBox->isChecked());

        QDialog::accept();
    }
}

void DhtDialog::reject()
{
    CloseApplicationDialog dialog;
    dialog.exec();
}

void DhtDialog::addButtonClicked()
{
    DhtEditDialog editDialog(this);
    if (editDialog.exec() == QDialog::Accepted) {
        Settings::DhtServer serverInfo = editDialog.getServerInformation();
        serverModel->appendRow(new QStandardItem(serverInfo.name));
        serverComboBox->setCurrentIndex(serverModel->rowCount() - 1);
        modifiedServerList << serverInfo;
    }
}

void DhtDialog::editButtonClicked()
{
    int currentIndex = serverComboBox->currentIndex();
    if (currentIndex == -1) {
        return;
    }

    DhtEditDialog editDialog(this);
    Settings::DhtServer oldServerInfo = modifiedServerList.at(currentIndex);
    editDialog.setServerInformation(oldServerInfo);
    if (editDialog.exec() == QDialog::Accepted) {
        Settings::DhtServer newServerInfo = editDialog.getServerInformation();
        serverModel->setData(serverModel->index(currentIndex, 0), newServerInfo.name);
        modifiedServerList[currentIndex] = newServerInfo;
    }
}

void DhtDialog::removeButtonClicked()
{
    int currentIndex = serverComboBox->currentIndex();
    if (currentIndex != -1) {
        serverModel->removeRow(currentIndex);
        modifiedServerList.removeAt(currentIndex);
    }
}
