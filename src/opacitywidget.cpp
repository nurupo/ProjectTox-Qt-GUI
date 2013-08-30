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

#include "opacitywidget.hpp"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

OpacityWidget::OpacityWidget(QWidget *parent) :
    QWidget(parent)
{
    // Opacity effect
    effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);

    // Animation
    animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(200);
    animation->setKeyValueAt(0, 0);
    animation->setKeyValueAt(1, 1);
    animation->setLoopCount(1);
}

qreal OpacityWidget::opacity() const
{
    return mOpacity;
}

void OpacityWidget::setOpacity(qreal arg)
{
    mOpacity = arg;
    effect->setOpacity(arg);
}

void OpacityWidget::showEvent(QShowEvent *e)
{
    animation->start();
    QWidget::showEvent(e);
}
