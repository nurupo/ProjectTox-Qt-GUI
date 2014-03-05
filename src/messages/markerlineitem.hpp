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

#ifndef MARKERLINEITEM_HPP
#define MARKERLINEITEM_HPP

#include <QGraphicsObject>
#include "chatscene.hpp"

class MarkerLineItem : public QGraphicsObject
{
    Q_OBJECT
public:
    MarkerLineItem(qreal sceneWidth, QGraphicsItem *parent = 0);
    virtual inline int type() const { return ChatScene::MarkerLineType; }

    inline QRectF boundingRect() const { return _boundingRect; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    inline ChatLine *chatLine() const { return _chatLine; }

public slots:
    //! Set the ChatLine this MarkerLineItem is associated to
    void setChatLine(ChatLine *line);
    void sceneRectChanged(const QRectF &rect);

private slots:
    void styleChanged();

private:
    QRectF _boundingRect;
    QBrush _brush;
    ChatLine *_chatLine;
};

#endif // MARKERLINEITEM_HPP
