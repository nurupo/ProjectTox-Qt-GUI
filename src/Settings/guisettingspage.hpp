/*
    Copyright (C) 2013 by Martin Kröll <technikschlumpf@web.de>

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

#ifndef GUISETTINGSPAGE_HPP
#define GUISETTINGSPAGE_HPP

#include "abstractsettingspage.hpp"
#include <QDebug>

class QGroupBox;
class QCheckBox;
class QComboBox;
class QLabel;
class QToolButton;
class EmojiFontSettingsDialog;

class GuiSettingsPage : public AbstractSettingsPage
{
    Q_OBJECT
public:
    GuiSettingsPage(QWidget *parent);

    void buildGui();
    void setGui();
    void applyChanges();
    
private slots:
    void updateSmileypackDetails(int index);

private:
    QGroupBox* buildAnimationGroup();
    QGroupBox* buildSmileypackGroup();

    void searchSmileyPacks(bool defaultPackDir);


    EmojiFontSettingsDialog *emojiSettings;
    QCheckBox* enableAnimationCheckbox;

    QComboBox* smileypackCombobox;
    QToolButton *emojiButton;
    QLabel*    smileypackDescLabel;
};


#endif // GUISETTINGSPAGE_HPP
