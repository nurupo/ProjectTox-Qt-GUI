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

#include "imagecombobox.hpp"

#include <QStylePainter>

ImageComboBox::ImageComboBox(QWidget* parent) :
    QComboBox(parent)
{
    setMinimumSize(16, 16);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFixedSize(54, 24);
}

void ImageComboBox::paintEvent(QPaintEvent* /*e*/)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    opt.currentText = "";
    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

QSize ImageComboBox::sizeHint() const
{
    return QSize(16, 16);
}
