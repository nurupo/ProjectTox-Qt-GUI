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

#include "starter.hpp"
#include "Settings/settings.hpp"

Starter::Starter(QObject* parent) :
    QObject(parent)
{
    createMainWindow();

/*
    if (Settings::getInstance().getDontShowDhtDialog())
    {
        createMainWindow();
    } else {
        dhtDialog = new DhtDialog();
        connect(dhtDialog, &DhtDialog::accepted, this, &Starter::onDhtDialogAccepted);
        dhtDialog->show();
    }
*/
}

void Starter::onDhtDialogAccepted()
{
    createMainWindow();

    dhtDialog->deleteLater();
}

void Starter::createMainWindow()
{
    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();
}
