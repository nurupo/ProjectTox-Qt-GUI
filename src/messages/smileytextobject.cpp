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

#include "smileytextobject.hpp"
#include <QPainter>


SmileyTextObject::SmileyTextObject(const QString &pixmap)
{
    mImage = QImage(pixmap);
}

QSizeF SmileyTextObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    Q_UNUSED(format)
    QSize size = mImage.size();

    if (size.height() > 25)
        size *= 25.0 / (double) size.height();

    return QSizeF(size);
}

void SmileyTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    Q_UNUSED(format)
    painter->drawImage(rect, mImage);
}
