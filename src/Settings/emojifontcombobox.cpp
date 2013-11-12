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

#include "emojifontcombobox.hpp"
#include <QFontDatabase>

EmojiFontComboBox::EmojiFontComboBox(QWidget *parent) :
    QComboBox(parent)
{
    QStringList emoticonFonts;
    QFontDatabase db;
    QFont f;
    for (QString &family : db.families()) {
        f.setFamily(family);
        QFontMetrics fm (f);
        // Emoticons, Range: 1F600–1F64F
        // http://www.unicode.org/charts/PDF/U1F600.pdf
        // check if a font has Emoticons support
        if (fm.inFontUcs4(GRINNING_CAT_FACE_WITH_SMILING_EYES)) {
            emoticonFonts << family;
        }
    }
    addItems(emoticonFonts);

    setEditable(true);
}
