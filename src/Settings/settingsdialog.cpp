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

#include "dhtbootstrapsettingspage.hpp"
#include "loggingsettingspage.hpp"
#include "settingsdialog.hpp"
#include "guisettingspage.hpp"
#include "soundsettingspage.hpp"

SettingsDialog::SettingsDialog(QWidget* parent) :
    BasicSettingsDialog(parent)
{
    setWindowTitle("Settings");

    addPage(":/icons/server.png", "DHT Bootstrap", new DhtBootstrapSettingsPage(this));
    //NOTE: make use of when logging will be implemented
    //addPage(":/icons/database.png", "Logging", new LoggingSettingsPage(this));
    addPage(":/icons/application_side_list.png", tr("GUI"), new GuiSettingsPage(this));
    addPage(":/icons/sound.png", tr("Sound"), new SoundSettingsPage(this));

    listWidget->setCurrentRow(0);
    listWidget->setMinimumWidth(130);
    setMinimumSize(450, 325);
}
