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

#ifndef OPACITYWIDGET_HPP
#define OPACITYWIDGET_HPP

#include <QWidget>

class QPropertyAnimation;
class QGraphicsOpacityEffect;

/*! A widget, whith fade effect and opacity option. */
class OpacityWidget : public QWidget
{
    Q_OBJECT
public:
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

    explicit OpacityWidget(QWidget *parent = 0);
    qreal opacity() const;
    
public slots:
    void setOpacity(qreal arg);

protected:
    void showEvent(QShowEvent *e);

private:
    QPropertyAnimation     *animation;
    QGraphicsOpacityEffect *effect;
    qreal                   mOpacity;
};

#endif // OPACITYWIDGET_HPP
