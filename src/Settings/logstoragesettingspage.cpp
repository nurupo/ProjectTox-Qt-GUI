/*
    Copyright (C) 2013 by Zack Mullaly <redwire@riseup.net>

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

#include "settings.hpp"
#include "logstoragesettingspage.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QFile>

LogStorageSettingsPage::LogStorageSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void LogStorageSettingsPage::buildGui()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QGroupBox* checkBoxGroup = buildLogSettingsGroup();

    layout->addWidget(checkBoxGroup);
}

void LogStorageSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    QFile storing(Settings::ABOUTLOGS);
    QString storeLogMsg;

    storeLogsBox->setChecked(settings.getStoreLogsSetting());
    encryptLogsBox->setChecked(settings.getEncryptLogsSetting());

    if (!storing.open(QIODevice::ReadOnly)) {
        storeLogMsg = "No information about log storage and encryption available.";
    } else {
        storeLogMsg = QString(storing.readAll());
    }
    storing.close();
    this->layout()->addWidget(new QLabel(storeLogMsg));
}

void LogStorageSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setStoreLogsSetting(storeLogsBox->isChecked());
    settings.setEncryptLogsSetting(encryptLogsBox->isChecked());
}

QGroupBox* LogStorageSettingsPage::buildLogSettingsGroup()
{
    QGroupBox* checkBoxGroup = new QGroupBox("Log Storage/Encryption", this);
    QVBoxLayout* layout = new QVBoxLayout(checkBoxGroup);

    storeLogsBox = new QCheckBox("Store logs", checkBoxGroup);
    encryptLogsBox = new QCheckBox("Encrypt logs", checkBoxGroup);

    connect(storeLogsBox,   &QCheckBox::clicked, this, &LogStorageSettingsPage::storeLogsBoxClicked);
    connect(encryptLogsBox, &QCheckBox::clicked, this, &LogStorageSettingsPage::encryptLogsBoxClicked);

    layout->addWidget(storeLogsBox);
    layout->addWidget(encryptLogsBox);

    return checkBoxGroup;
}

void LogStorageSettingsPage::storeLogsBoxClicked()
{
    if (!storeLogsBox->isChecked()) {
        encryptLogsBox->setChecked(false);
        encryptLogsBox->setCheckable(false);
    } else {
        encryptLogsBox->setCheckable(true);
    }
}

void LogStorageSettingsPage::encryptLogsBoxClicked()
{
    if (!storeLogsBox->isChecked()) {
        encryptLogsBox->setChecked(false);
    }
}
