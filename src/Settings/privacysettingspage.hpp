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

#ifndef PRIVACYSETTINGSPAGE_H
#define PRIVACYSETTINGSPAGE_H

#include "abstractsettingspage.hpp"

class QGroupBox;
class QCheckBox;

class PrivacySettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    PrivacySettingsPage(QWidget *parent);

    void buildGui();
    void setGui();
    void applyChanges();

signals:

public slots:

private:
    QGroupBox *buildTypingGroup();
    QCheckBox *mTypingCheckbox;
};

#endif // PRIVACYSETTINGSPAGE_H
