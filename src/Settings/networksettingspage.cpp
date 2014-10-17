/*
    Copyright (C) 2014 by Maxim Biro <nurupo.contributions@gmail.com>

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

#include "networksettingspage.hpp"

#include "informationiconlabel.hpp"
#include "settings.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>

NetworkSettingsPage::NetworkSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void NetworkSettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QGroupBox *networkGroup = buildNetworkGroup();

    layout->addWidget(networkGroup);
    layout->addStretch(0);
}

QGroupBox* NetworkSettingsPage::buildNetworkGroup()
{
    QGroupBox *group = new QGroupBox("Network", this);
    QVBoxLayout *layout = new QVBoxLayout(group);

    enableIPv6CheckBox = new QCheckBox("Use IPv6", group);

    InformationIconLabel *IPv6InfoIcon = new InformationIconLabel(group);
    IPv6InfoIcon->setToolTip("Change requires client restart in order to take place");

    enableIPv4FallbackCheckBox = new QCheckBox("Fallback to IPv4", group);

    connect(enableIPv6CheckBox, &QCheckBox::stateChanged, this, &NetworkSettingsPage::IPv6CheckBoxStateChanged);

    QHBoxLayout *IPv6Layout = new QHBoxLayout();
    IPv6Layout->setSpacing(0);

    IPv6Layout->addWidget(enableIPv6CheckBox);
    IPv6Layout->addWidget(IPv6InfoIcon, 999, Qt::AlignHCenter | Qt::AlignLeft);

    layout->addLayout(IPv6Layout);
    layout->addWidget(enableIPv4FallbackCheckBox);

    return group;
}

void NetworkSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    enableIPv6CheckBox->setChecked(settings.isIPv6Enabled());
    enableIPv4FallbackCheckBox->setChecked(settings.isIPv4FallbackEnabled());
    IPv6CheckBoxStateChanged(enableIPv6CheckBox->isChecked() ? Qt::Checked : Qt::Unchecked);
}

void NetworkSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setIPv6Enabled(enableIPv6CheckBox->isChecked());
    settings.setIPv4FallbackEnabled(enableIPv4FallbackCheckBox->isChecked());
}

void NetworkSettingsPage::IPv6CheckBoxStateChanged(int state) {
    enableIPv4FallbackCheckBox->setEnabled(state == Qt::Checked);
}

