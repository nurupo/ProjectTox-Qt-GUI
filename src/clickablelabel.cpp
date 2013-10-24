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

#include "clickablelabel.hpp"

#include <QApplication>
#include <QEvent>
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) :
    ElideLabel(parent)
{
}

bool ClickableLabel::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit clicked();
        }
    } else if (event->type() == QEvent::Enter) {
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
    } else if (event->type() == QEvent::Leave) {
        QApplication::restoreOverrideCursor();
    }

    return ElideLabel::event(event);
}
