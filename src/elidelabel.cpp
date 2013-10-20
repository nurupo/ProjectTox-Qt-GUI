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

#include "elidelabel.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

ElideLabel::ElideLabel(QWidget *parent) :
    QLabel(parent), _textElide(false), _textElideMode(Qt::ElideNone)
{
    this->installEventFilter(this);
}

void ElideLabel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QFontMetrics metrics(font());
    if ((metrics.width(text()) > contentsRect().width()) && textElide()) {
        QString elidedText = fontMetrics().elidedText(text(), textElideMode(), rect().width());
        p.drawText(rect(), elidedText);
    } else {
        QLabel::paintEvent(event);
    }
}

bool ElideLabel::eventFilter(QObject *obj, QEvent *ev)
{
    switch(ev->type()) {
    case QEvent::MouseButtonRelease:
        emit clicked();
        break;
    case QEvent::Enter:
        QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
        break;
    case QEvent::Leave:
        QApplication::restoreOverrideCursor();
        break;
    default:
        return false;
    }

    return true;    //will have returned false already if event was unhandled by the switch statement
}

void ElideLabel::setTextElide(bool set)
{
    _textElide = set;
}

bool ElideLabel::textElide() const
{
    return _textElide;
}

void ElideLabel::setTextElideMode(Qt::TextElideMode mode)
{
    _textElideMode = mode;
}

Qt::TextElideMode ElideLabel::textElideMode() const
{
    return _textElideMode;
}
