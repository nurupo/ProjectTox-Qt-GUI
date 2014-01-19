#include "markerlineitem.hpp"
#include <QPainter>

MarkerLineItem::MarkerLineItem(qreal sceneWidth, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _boundingRect(0, 0, sceneWidth, 1),
    _chatLine(0)
{
    setVisible(false);
    setZValue(8);
    styleChanged(); // init brush and height
}

void MarkerLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(boundingRect(), _brush);
}

void MarkerLineItem::setChatLine(ChatLine *line)
{
    _chatLine = line;
    if (!line)
        setVisible(false);
}

void MarkerLineItem::sceneRectChanged(const QRectF &rect)
{
    prepareGeometryChange();
    _boundingRect.setWidth(rect.width());
}

void MarkerLineItem::styleChanged()
{
    _brush = QBrush(Qt::red);

       // if this is a solid color, we assume 1px because wesurely  don't surely don't want to fill the entire chatline.
       // else, use the height of a single line of text to play around with gradients etc.
       qreal height = 1.;

       prepareGeometryChange();
       _boundingRect = QRectF(0, 0, scene() ? scene()->width() : 100, height);
}
