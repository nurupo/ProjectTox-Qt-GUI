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

#ifndef COLUMNHANDLEITEM_HPP
#define COLUMNHANDLEITEM_HPP

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include "chatscene.hpp"

class ColumnHandleItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    ColumnHandleItem(qreal width, QGraphicsItem *parent = 0);
    virtual inline int type() const { return ChatScene::ColumnHandleType; }

    inline qreal width() const { return _width; }
    inline QRectF boundingRect() const { return _boundingRect; }
    inline qreal sceneLeft() const { return _sceneLeft; }
    inline qreal sceneRight() const { return _sceneRight; }

    inline qreal opacity() const { return _opacity; }

    void setXPos(qreal xpos);
    void setXLimits(qreal min, qreal max);
    inline qreal xMin() { return _minXPos; }
    inline qreal xMax() { return _maxXPos; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void sceneRectChanged(const QRectF &rect);
    void setOpacity(qreal opacity);

signals:
    void positionChanged(qreal x);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    qreal _width;
    qreal _sceneLeft, _sceneRight;
    QRectF _boundingRect;
    bool _moving;
    qreal _offset;
    qreal _minXPos, _maxXPos;
    qreal _opacity;
    QPropertyAnimation *_animation;
};

#endif // COLUMNHANDLEITEM_HPP
