/*
    Copyright (C) 2014 by Martin Kröll <technikschlumpf@web.de>

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

#include "privacysettingspage.hpp"
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include "settings.hpp"

PrivacySettingsPage::PrivacySettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void PrivacySettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(buildTypingGroup());
    layout->addStretch(0);
}

void PrivacySettingsPage::setGui()
{
    const Settings &settings = Settings::getInstance();
    mTypingCheckbox->setChecked(settings.isTypingNotificationEnabled());
}

void PrivacySettingsPage::applyChanges()
{
    Settings &settings = Settings::getInstance();
    settings.setTypingNotification(mTypingCheckbox->isChecked());
}

QGroupBox *PrivacySettingsPage::buildTypingGroup()
{
    QGroupBox *group = new QGroupBox(tr("Typing notification"), this);
    QVBoxLayout *layout = new QVBoxLayout(group);
    mTypingCheckbox = new QCheckBox(tr("Show others when I'm typing"), group);
    layout->addWidget(mTypingCheckbox);
    return group;
}
