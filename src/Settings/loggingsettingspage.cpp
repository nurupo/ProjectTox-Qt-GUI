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

#include "loggingsettingspage.hpp"

#include "informationiconlabel.hpp"
#include "settings.hpp"

#include <QLabel>
#include <QVBoxLayout>

LoggingSettingsPage::LoggingSettingsPage(QWidget* parent) :
    AbstractSettingsPage(parent)
{
}

void LoggingSettingsPage::buildGui()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    loggingGroup = buildLoggingGroup();

    layout->addWidget(loggingGroup);
    layout->addStretch(0);
}

QGroupBox* LoggingSettingsPage::buildLoggingGroup()
{
    QGroupBox* group = new QGroupBox("Logging", this);
    group->setCheckable(true);
    QVBoxLayout* layout = new QVBoxLayout(group);

    encryptLogsCheckBox = new QCheckBox("Encrypt logs", group);

    InformationIconLabel* encryptLogsInfoLabel = new InformationIconLabel(group);
    encryptLogsInfoLabel->setToolTip(QString("%1\n%2").arg("It is highly recommended that you encrypt logs as a countermeasure to logs being stolen.", "Don't disable this option unless you have really good reasons for doing so."));

    QHBoxLayout* encryptLogsLayout = new QHBoxLayout();

    encryptLogsLayout->addWidget(encryptLogsCheckBox);
    encryptLogsLayout->addWidget(encryptLogsInfoLabel, 999, Qt::AlignHCenter | Qt::AlignLeft);

    layout->addLayout(encryptLogsLayout);

    return group;
}

void LoggingSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    loggingGroup->setChecked(settings.getEnableLogging());
    encryptLogsCheckBox->setChecked(settings.getEncryptLogs());
}

void LoggingSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setEnableLogging(loggingGroup->isChecked());
    settings.setEncryptLogs(encryptLogsCheckBox->isChecked());
}
