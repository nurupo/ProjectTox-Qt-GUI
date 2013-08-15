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

#ifndef DHTBOOTSTRAPSETTINGSPAGE_HPP
#define DHTBOOTSTRAPSETTINGSPAGE_HPP

#include "abstractsettingspage.hpp"
#include "settings.hpp"

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>

class DhtBootstrapSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    DhtBootstrapSettingsPage(QWidget* parent);

    void applyChanges();
    void buildGui();
    void setGui();

private:
    QGroupBox* buildServerGroup();

    QHash<int, Settings::DhtServer> serverHash;
    int uniqueKey;
    QStandardItemModel* serverListModel;
    QSortFilterProxyModel* serverListProxyModel;
    QTreeView* serverListView;
    bool serverListIsDirty;

private slots:
    void serverAddButtonClicked();
    void serverRemoveButtonClicked();
    void serverEditButtonClicked();

};

#endif // DHTBOOTSTRAPSETTINGSPAGE_HPP
