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
