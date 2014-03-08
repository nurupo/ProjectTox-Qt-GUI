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

#include "soundsettingspage.hpp"
#include "settings.hpp"
#include "soundmanager.hpp"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDir>
#include <QDirIterator>
#include <QCheckBox>
#include <QLabel>

SoundSettingsPage::SoundSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void SoundSettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(buildSoundpackGroup());
    layout->addStretch(0);
}

void SoundSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    enableGroup->setChecked(settings.isSoundEnabled());
    searchSoundPacks();
    playOnError->setChecked(settings.isPlayOnError());
    playOnLogInOut->setChecked(settings.isPlayOnLogInOut());
    playOnFriendLogsInOut->setChecked(settings.isPlayOnFriendLogsInOut());
    playOnFriendRequestAccepted->setChecked(settings.isPlayOnFriendRequestAccepted());
    playOnIncommingCall->setChecked(settings.isPlayOnIncommingCall());
    playOnOutgoingCall->setChecked(settings.isPlayOnOutgoingCall());
    playOnNewMessage->setChecked(settings.isPlayOnNewMessage());
    playOnFiletransferComplete->setChecked(settings.isPlayOnFiletransferComplete());
    playOnFiletransferPending->setChecked(settings.isPlayOnFiletransferPending());
}

void SoundSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setSoundEnabled(enableGroup->isChecked());
    settings.setPlayOnError(playOnError->isChecked());
    settings.setPlayOnLogInOut(playOnLogInOut->isChecked());
    settings.setPlayOnFriendLogsInOut(playOnFriendLogsInOut->isChecked());
    settings.setPlayOnFriendRequestAccepted(playOnFriendRequestAccepted->isChecked());
    settings.setPlayOnIncommingCall(playOnIncommingCall->isChecked());
    settings.setPlayOnOutgoingCall(playOnOutgoingCall->isChecked());
    settings.setPlayOnNewMessage(playOnNewMessage->isChecked());
    settings.setPlayOnFiletransferComplete(playOnFiletransferComplete->isChecked());
    settings.setPlayOnFiletransferPending(playOnFiletransferPending->isChecked());
}

QGroupBox *SoundSettingsPage::buildSoundpackGroup()
{
    enableGroup = new QGroupBox(tr("Enable sounds"), this);
    enableGroup->setCheckable(true);

    QFormLayout* packLayout = new QFormLayout;
    packCombobox = new QComboBox(enableGroup);
    packLayout->addRow(tr("Theme:"), packCombobox);

    playOnError = new QCheckBox(tr("Error"), enableGroup);
    playOnLogInOut = new QCheckBox(tr("Login/Logout"), enableGroup);
    playOnFriendLogsInOut = new QCheckBox(tr("Friend logs in/out"), enableGroup);
    playOnFriendRequestAccepted = new QCheckBox(tr("Friend request accepted"), enableGroup);
    playOnIncommingCall = new QCheckBox(tr("Incomming call / video call"), enableGroup);
    playOnOutgoingCall = new QCheckBox(tr("Outgoing call / video call"), enableGroup);
    playOnNewMessage = new QCheckBox(tr("New message"), enableGroup);
    playOnFiletransferComplete = new QCheckBox(tr("Filetransfer complete"), enableGroup);
    playOnFiletransferPending = new QCheckBox(tr("Filetransfer pending"), enableGroup);

    QVBoxLayout *playLayout = new QVBoxLayout;
    playLayout->setSpacing(0);
    playLayout->setContentsMargins(10,0,0,0);
    playLayout->addWidget(playOnError);
    playLayout->addWidget(playOnLogInOut);
    playLayout->addWidget(playOnFriendLogsInOut);
    playLayout->addWidget(playOnFriendRequestAccepted);
    playLayout->addWidget(playOnIncommingCall);
    playLayout->addWidget(playOnOutgoingCall);
    playLayout->addWidget(playOnNewMessage);
    playLayout->addWidget(playOnFiletransferComplete);
    playLayout->addWidget(playOnFiletransferPending);

    QVBoxLayout *layout = new QVBoxLayout(enableGroup);
    layout->addLayout(packLayout);
    layout->addWidget(new QLabel(tr("Play sound on..."), playOnError));
    layout->addLayout(playLayout);

    return enableGroup;
}

void SoundSettingsPage::searchSoundPacks()
{
    // Go to sound pack folder
    QDir dir(SoundManager::directory());
    if (!dir.exists())
        return;

    // Go through all packs
    dir.setFilter(QDir::Dirs|QDir::NoDot|QDir::NoDotDot);
    QDirIterator it(dir);
    while (it.hasNext()) {
        it.next();

        // Check theme file
        QFileInfo f(it.filePath());
        if (!f.exists())
            continue;

        // Parse theme file
        SoundPack newPack;
        if (!newPack.parse(f.baseName()))
            continue;

        // Add new pack to combobox
        packCombobox->addItem(tr("%1 by %2").arg(newPack.name(), newPack.author()), newPack.themeDirName());
    }
}
