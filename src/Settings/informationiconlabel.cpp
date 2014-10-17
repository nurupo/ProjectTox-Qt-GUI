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

#include "informationiconlabel.hpp"

#include <QPainter>

InformationIconLabel::InformationIconLabel(QWidget *parent) :
    QLabel(parent)
{
    QImage image(":/icons/information.png");

    // set alpha to 175
    QPainter p;
    p.begin(&image);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(image.rect(), QColor(0, 0, 0, 175));
    p.end();

    setPixmap(QPixmap::fromImage(image));
    setFixedSize(16, 16);
}
