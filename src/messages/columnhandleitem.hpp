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
