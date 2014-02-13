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

#include "../customhinttreeview.hpp"
#include "dhtbootstrapsettingspage.hpp"
#include "dhtserverdialog.hpp"

#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>

DhtBootstrapSettingsPage::DhtBootstrapSettingsPage(QWidget* parent) :
    AbstractSettingsPage(parent), serverListIsDirty(false)
{
}

void DhtBootstrapSettingsPage::buildGui()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* serverGroup = buildServerGroup();

    layout->addWidget(serverGroup);
}

QGroupBox* DhtBootstrapSettingsPage::buildServerGroup()
{
    QGroupBox* group = new QGroupBox("DHT Bootstrap Servers", this);
    QGridLayout* layout = new QGridLayout(group);

    serverListModel = new QStandardItemModel(group);
    serverListModel->setHorizontalHeaderLabels(QStringList() << "Name");

    serverListView = new CustomHintTreeView(group, QSize(10, 10));
    serverListView->setModel(serverListModel);
    serverListView->setIndentation(0);
    serverListView->setSortingEnabled(true);
    serverListView->header()->setSortIndicator(0, Qt::AscendingOrder);

    QPushButton* serverAddButton = new QPushButton("Add", group);
    QPushButton* serverRemoveButton = new QPushButton("Remove", group);
    QPushButton* serverEditButton = new QPushButton("Edit", group);

    connect(serverAddButton,    &QPushButton::clicked, this, &DhtBootstrapSettingsPage::serverAddButtonClicked);
    connect(serverRemoveButton, &QPushButton::clicked, this, &DhtBootstrapSettingsPage::serverRemoveButtonClicked);
    connect(serverEditButton,   &QPushButton::clicked, this, &DhtBootstrapSettingsPage::serverEditButtonClicked);

    layout->addWidget(serverListView,     0, 0, 6, 3);
    layout->addWidget(serverAddButton,    0, 3, 1, 1);
    layout->addWidget(serverEditButton,   1, 3, 1, 1);
    layout->addWidget(serverRemoveButton, 2, 3, 1, 1);

    return group;
}

void DhtBootstrapSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    const QList<Settings::DhtServer>& serverList = settings.getDhtServerList();
    uniqueKey = 0;
    for (const Settings::DhtServer& server : serverList) {
        QStandardItem* serverItem = new QStandardItem(server.name);
        serverItem->setEditable(false);
        serverItem->setData(uniqueKey);
        serverListModel->appendRow(serverItem);

        serverHash[uniqueKey] = server;
        uniqueKey++;
    }
    serverListModel->sort(0, Qt::AscendingOrder);
}

void DhtBootstrapSettingsPage::applyChanges()
{
    if (!serverListIsDirty) {
        return;
    }

    Settings& settings = Settings::getInstance();
    QList<Settings::DhtServer> serverList;

    QHashIterator<int, Settings::DhtServer> i(serverHash);
    while (i.hasNext()) {
        i.next();
        serverList << i.value();
    }

    settings.setDhtServerList(serverList);
}

void DhtBootstrapSettingsPage::serverAddButtonClicked()
{
    DhtServerDialog serverInfoDialog(this);
    if (serverInfoDialog.exec() == QDialog::Accepted) {
        Settings::DhtServer serverInfo = serverInfoDialog.getServerInformation();
        QStandardItem* name = new QStandardItem(serverInfo.name);
        name->setEditable(false);
        name->setData(uniqueKey);
        serverHash[uniqueKey] = serverInfo;
        uniqueKey++;
        name->setFlags(name->flags() & ~Qt::ItemIsEditable);
        serverListModel->appendRow(name);
        serverListView->setCurrentIndex(serverListModel->indexFromItem(name));
        serverListModel->sort(0, Qt::AscendingOrder);
        serverListIsDirty = true;
    }
}

void DhtBootstrapSettingsPage::serverRemoveButtonClicked()
{
    QModelIndex currentIndex = serverListView->currentIndex();
    if (currentIndex != QModelIndex()) {
        QList<QStandardItem*> removedRow = serverListModel->takeRow(currentIndex.row());
        serverHash.remove(removedRow.at(0)->data().toInt());
        qDeleteAll(removedRow);
        serverListIsDirty = true;
    }
}

void DhtBootstrapSettingsPage::serverEditButtonClicked()
{
    QModelIndex currentIndex = serverListView->currentIndex();
    if (currentIndex == QModelIndex()) {
        return;
    }

    DhtServerDialog serverInfoDialog(this);
    const int id = serverListModel->itemFromIndex(currentIndex)->data().toInt();
    Settings::DhtServer oldServerInfo = serverHash[id];
    serverInfoDialog.setServerInformation(oldServerInfo);
    if (serverInfoDialog.exec() == QDialog::Accepted) {
        Settings::DhtServer newServerInfo = serverInfoDialog.getServerInformation();
        serverListModel->setData(currentIndex, newServerInfo.name);
        serverHash[id] = newServerInfo;
        serverListModel->sort(0, Qt::AscendingOrder);
        serverListIsDirty = true;
    }
}
