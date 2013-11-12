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

#ifndef EMOJIFONTCOMBOBOX_HPP
#define EMOJIFONTCOMBOBOX_HPP

#include <QComboBox>

class EmojiFontComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit EmojiFontComboBox(QWidget *parent = 0);

    static const uint GRINNING_CAT_FACE_WITH_SMILING_EYES = 0x1F638;
signals:
    
public slots:
    
};

#endif // EMOJIFONTCOMBOBOX_HPP
