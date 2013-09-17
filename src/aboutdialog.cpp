/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>

    This file is part of NFK Lobby.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

#include "aboutdialog.hpp"
#include "appinfo.hpp"

#include <QApplication>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QGridLayout* layout = new QGridLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    const QString Url("<a href=\"%2\">%1</a>");

    applicationNameLabel->setText(AppInfo::name);
    versionLabel->setText(QString("version %1").arg(AppInfo::version));
    buildDateLabel->setText(QString("Build date: %1").arg(AppInfo::buildDate));
    iconsSourceLabel->setText(QString("%1 %2").arg("This application uses").arg(Url.arg("FatCow icons").arg("http://www.fatcow.com/free-icons")));
    licenseLabel->setText(QString("License: %1").arg(AppInfo::license));
    copyrightLabel->setText(QString("(c) %1, %2").arg(AppInfo::years).arg(Url.arg(AppInfo::author).arg(AppInfo::Url)));
}
