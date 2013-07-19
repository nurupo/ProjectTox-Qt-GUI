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

#ifndef BASICSETTINGSDIALOG_HPP
#define BASICSETTINGSDIALOG_HPP

#include "abstractsettingspage.hpp"

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

class BasicSettingsDialog : public QDialog
{
public:
    explicit BasicSettingsDialog(QWidget* parent);
    virtual ~BasicSettingsDialog();
    void accept();

protected:

    QListWidget* listWidget;
    QStackedWidget* stackedWidget;

    void addPage(const QString& iconPath, const QString& name, AbstractSettingsPage* page);
};

#endif // BASICSETTINGSDIALOG_HPP
