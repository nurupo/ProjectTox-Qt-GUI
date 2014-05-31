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

#include "typingitem.hpp"
#include <QPainter>
#include <QDebug>

TypingItem::TypingItem(qreal sceneWidth, QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    hide();
    connect(this, &TypingItem::visibleChanged, this, &TypingItem::onVisibleChanged);

    mFont.setPointSize(mFont.pointSize() - 1);
    mFont.setItalic(true);
    QFontMetricsF fm(mFont);

    mBoundingRect = QRectF(0, 0, sceneWidth, fm.height());

    mPoints = 0;
    mPointTimer.setInterval(300);
    connect(&mPointTimer, &QTimer::timeout, this, &TypingItem::onPointTimeout);
}

void TypingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(mFont);
    painter->drawText(boundingRect().adjusted(10,0,-20,0), tr("%1 is typing%2").arg(mName, mPointStr));

}

void TypingItem::setVisible(const QString &name)
{
    mName = name;
    show();
}

void TypingItem::sceneRectChanged(const QRectF &rect)
{
    prepareGeometryChange();
    mBoundingRect.setWidth(rect.width());
}

void TypingItem::onVisibleChanged()
{
    if(isVisible())
        mPointTimer.start();
    else
        mPointTimer.stop();
}

void TypingItem::onPointTimeout()
{
    if (mPoints > 2) {
        mPoints = 0;
        mPointStr.clear();
    }
    else {
        mPointStr.append('.');
        mPoints++;
    }

    update();
}
