/*
    Copyright (C) 2005-2014 by the Quassel Project <devel@quassel-irc.org>
                  2014 by Martin Kröll <technikschlumpf@web.de>

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

#include "markerlineitem.hpp"
#include <QPainter>

MarkerLineItem::MarkerLineItem(qreal sceneWidth, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _boundingRect(0, 0, sceneWidth, 1),
    _chatLine(0)
{
    setVisible(false);
    setZValue(8);
    styleChanged(); // init brush and height
}

void MarkerLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(boundingRect(), _brush);
}

void MarkerLineItem::setChatLine(ChatLine *line)
{
    _chatLine = line;
    if (!line)
        setVisible(false);
}

void MarkerLineItem::sceneRectChanged(const QRectF &rect)
{
    prepareGeometryChange();
    _boundingRect.setWidth(rect.width());
}

void MarkerLineItem::styleChanged()
{
    _brush = QBrush(QColor(QStringLiteral("#6BC25F")));
    qreal height = 1.0;

    prepareGeometryChange();
    _boundingRect = QRectF(0, 0, scene() ? scene()->width() : 100, height);
}
