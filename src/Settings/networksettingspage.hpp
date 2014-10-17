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

#ifndef NETWORKSETTINGSPAGE_HPP
#define NETWORKSETTINGSPAGE_HPP

#include "abstractsettingspage.hpp"

class QGroupBox;
class QCheckBox;

class NetworkSettingsPage : public AbstractSettingsPage
{
public:
    NetworkSettingsPage(QWidget *parent);

    void applyChanges();
    void buildGui();
    void setGui();

private:
    QGroupBox *buildNetworkGroup();

    QCheckBox *enableIPv6CheckBox;
    QCheckBox *enableIPv4FallbackCheckBox;

private slots:
    void IPv6CheckBoxStateChanged(int state);
};

#endif // NETWORKSETTINGSPAGE_HPP
