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

#ifndef TYPINGITEM_HPP
#define TYPINGITEM_HPP

#include <QGraphicsObject>
#include "chatscene.hpp"

class TypingItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit TypingItem(qreal sceneWidth, QGraphicsItem *parent = 0);
    virtual inline int type() const { return ChatScene::TypingNotificationType; }

    inline QRectF boundingRect() const { return mBoundingRect; }
    inline qreal height() const {return boundingRect().height(); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setVisible(const QString &name);

public slots:
    void sceneRectChanged(const QRectF &rect);

private slots:
    void onVisibleChanged();
    void onPointTimeout();

private:
    QRectF mBoundingRect;
    QFont mFont;

    QString mName;

    QTimer  mPointTimer;
    quint8  mPoints;
    QString mPointStr;
};

#endif // TYPINGITEM_HPP
