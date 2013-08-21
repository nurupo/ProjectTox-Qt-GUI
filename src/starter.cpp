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

class StarterPrivate
{
public:
    void createMainWindow();

    MainWindow *mainWindow;
};

void StarterPrivate::createMainWindow()
{
    delete mainWindow;
    mainWindow = new MainWindow();
    mainWindow->show();
}

Starter::Starter(QObject* parent) :
    QObject(parent),
    d_ptr(new StarterPrivate)
{
    Q_D(Starter);

    d->mainWindow = 0;
    d->createMainWindow();
}

Starter::~Starter()
{
    Q_D(Starter);
    delete d->mainWindow;
    delete d;
}

void Starter::onDhtDialogAccepted()
{
    Q_D(Starter);
    d->createMainWindow();
}

