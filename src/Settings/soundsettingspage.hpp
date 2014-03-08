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

#ifndef SOUNDSETTINGSPAGE_HPP
#define SOUNDSETTINGSPAGE_HPP

#include "abstractsettingspage.hpp"

class QGroupBox;
class QComboBox;
class QCheckBox;

class SoundSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    SoundSettingsPage(QWidget *parent);

    void buildGui();
    void setGui();
    void applyChanges();

private:
    QGroupBox* buildSoundpackGroup();

    void searchSoundPacks();

    QGroupBox *enableGroup;
    QComboBox *packCombobox;
    QCheckBox *playOnError;
    QCheckBox *playOnLogInOut;
    QCheckBox *playOnFriendLogsInOut;
    QCheckBox *playOnFriendRequestAccepted;
    QCheckBox *playOnIncommingCall;
    QCheckBox *playOnOutgoingCall;
    QCheckBox *playOnNewMessage;
    QCheckBox *playOnFiletransferComplete;
    QCheckBox *playOnFiletransferPending;
};

#endif // SOUNDSETTINGSPAGE_HPP
