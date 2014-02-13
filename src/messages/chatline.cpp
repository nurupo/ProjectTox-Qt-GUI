#include "chatline.hpp"
#include "chatview.hpp"
#include "chatitem.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QApplication>

ChatLine::ChatLine(int row, QAbstractItemModel *model, const qreal &width, const qreal &firstWidth, const qreal &secondWidth, const qreal &thirdWidth, const QPointF &secondPos, const QPointF &thirdPos, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    _row(row), // needs to be set before the items
    _model(model),
    _contentsItem(secondPos, secondWidth, this),
    _senderItem(QRectF(0, 0, firstWidth, _contentsItem.height()), this),
    _timestampItem(QRectF(thirdPos, QSizeF(thirdWidth, _contentsItem.height())), this),
    _width(width),
    _height(_contentsItem.height()),
    _selection(0),
    _mouseGrabberItem(0),
    _hoverItem(0)
{
    Q_ASSERT(model);
    setZValue(0);
    setAcceptHoverEvents(true);

    QModelIndex index = model->index(row, MessageModel::ContentsColumn);
    setHighlighted(index.data(MessageModel::FlagsRole).toInt() & Message::Highlight);
}

ChatLine::~ChatLine()
{
    if (chatView())
        chatView()->setHasCache(this, false);
}

ChatItem *ChatLine::item(MessageModel::ColumnType column)
{
    switch (column) {
    case MessageModel::TimestampColumn:
        return &_timestampItem;
    case MessageModel::SenderColumn:
        return &_senderItem;
    case MessageModel::ContentsColumn:
        return &_contentsItem;
    default:
        return 0;
    }
}

ChatItem *ChatLine::itemAt(const QPointF &pos)
{
    if (_contentsItem.boundingRect().contains(pos))
        return &_contentsItem;
    if (_senderItem.boundingRect().contains(pos))
        return &_senderItem;
    if (_timestampItem.boundingRect().contains(pos))
        return &_timestampItem;
    return 0;
}

void ChatLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const QAbstractItemModel *model_ = model();
    QModelIndex myIdx = model_->index(row(), 0);
    Message::Type type = (Message::Type)myIdx.data(MessageModel::TypeRole).toInt();
    UiStyle::MessageLabel label = (UiStyle::MessageLabel)myIdx.data(MessageModel::MsgLabelRole).toInt();

    QTextCharFormat msgFmt = UiStyle::getInstance().format(UiStyle::formatType(type), label);
    if (msgFmt.hasProperty(QTextFormat::BackgroundBrush)) {
        painter->fillRect(boundingRect(), msgFmt.background());
    }

    // Draw a rect for the whole selection (all colums with seperators).
    // TODO MKO Double selection drawing? See ChatItem::paintBackground.
    if (_selection & Selected) {
        QTextCharFormat selFmt = UiStyle::getInstance().format(UiStyle::formatType(type), label | UiStyle::Selected);
        if (selFmt.hasProperty(QTextFormat::BackgroundBrush)) {
            ChatItem *rightItem = item((MessageModel::ColumnType)((_selection & 0x0C)>>2));
            qreal right = rightItem->pos().x() + rightItem->width();
            qreal left  = item((MessageModel::ColumnType)(_selection & 0x03))->pos().x();
            QRectF selectRect(left, 0, right - left, height());
            painter->fillRect(selectRect, selFmt.background());
        }
    }

    // draw chatitems
    // the items draw themselves at the correct position
    senderItem()->paint(painter, option, widget);
    contentsItem()->paint(painter, option, widget);
    timestampItem()->paint(painter, option, widget);

    // draw seperator line
    // TODO MKO is this the right place for drawing the line? Maybe it's better to have a "seperatorline" object.
    if(row()-1 >= 0) {
        QModelIndex lastIdx = model_->index(row()-1, 0);
        if ((model_->data(lastIdx, MessageModel::FlagsRole).toInt() & Message::Self) != (model_->data(myIdx, MessageModel::FlagsRole).toInt() & Message::Self)) {
            painter->save();
            painter->setPen(QApplication::palette().button().color());
            painter->drawLine(0, 0, width(), 0);
            painter->restore();
        }
    }
}

void ChatLine::setFirstColumn(const qreal &firstWidth, const qreal &secondwidth, const QPointF &secondPos, qreal &linePos)
{
    // linepos is the *bottom* position for the line
    qreal height = _contentsItem.setGeometryByWidth(secondwidth);
    _contentsItem.setPos(secondPos);
    linePos -= height;
    bool needGeometryChange = (height != _height);

    _senderItem.setGeometry(firstWidth, _height);
    _timestampItem.setHeight(height);

    if (needGeometryChange)
        prepareGeometryChange();

    _height = height;

    setPos(0, linePos);
}

void ChatLine::setSecondColumn(const qreal &secondWidth, const qreal &thirdWidth, const QPointF &thirdPos, qreal &linePos)
{
    // linepos is the *bottom* position for the line
    qreal height = _contentsItem.setGeometryByWidth(secondWidth);
    linePos -= height;
    bool needGeometryChange = (height != _height);

    _senderItem.setHeight(height);
    _timestampItem.setGeometry(thirdWidth, height);
    _timestampItem.setPos(thirdPos);

    if (needGeometryChange)
        prepareGeometryChange();

    _height = height;

    setPos(0, linePos);
}

void ChatLine::setGeometryByWidth(const qreal &width, const qreal &thirdWidth, qreal &linePos)
{
    Q_UNUSED(width)
    Q_UNUSED(linePos)
    // linepos is the *bottom* position for the line
    _timestampItem.setWidth(thirdWidth);
        /*qreal height = _contentsItem.setGeometryByWidth(contentsWidth);
        linePos -= height;
        bool needGeometryChange = (height != _height || width != _width);

        if (height != _height) {
            _timestampItem.setHeight(height);
            _senderItem.setHeight(height);
        }

        if (needGeometryChange) {
            prepareGeometryChange();
            _height = height;
            _width = width;
        }

        setPos(0, linePos); // set pos is _very_ cheap if nothing changes.*/
}

void ChatLine::setSelected(bool selected, MessageModel::ColumnType minColumn, MessageModel::ColumnType maxColumn)
{
    if (selected) {
        quint8 sel = (_selection & Highlighted) | Selected | minColumn | (maxColumn<<2);
        if (sel != _selection) {
            _selection = sel;
            for (int i = 0; i < minColumn; i++) {
                item((MessageModel::ColumnType)i)->clearSelection();

            }
            for (int i = minColumn; i <= maxColumn; i++) {
                item((MessageModel::ColumnType)i)->setFullSelection();

            }
            for(int i = maxColumn+1; i<= MessageModel::TimestampColumn; i++) {
                item((MessageModel::ColumnType)i)->clearSelection();
            }
            update();
        }
    }
    else {
        quint8 sel = _selection & Highlighted;
        if (sel != _selection) {
            _selection = sel;
            for (int i = 0; i <= MessageModel::TimestampColumn; i++)
                item((MessageModel::ColumnType)i)->clearSelection();
            update();
        }
    }
}

void ChatLine::setHighlighted(bool highlighted)
{
    if (highlighted) _selection |= Highlighted;
    else _selection &= ~Highlighted;
    update();
}

void ChatLine::clearCache()
{
    _timestampItem.clearCache();
    _senderItem.clearCache();
    _contentsItem.clearCache();
}

bool ChatLine::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::GrabMouse) {
        // get mouse cursor pos relative to us
        ChatView *view = chatScene()->chatView();
        QPointF linePos = mapFromScene(view->mapToScene(view->mapFromGlobal(QCursor::pos())));
        setMouseGrabberItem(itemAt(linePos));
    }
    else if (event->type() == QEvent::UngrabMouse) {
        setMouseGrabberItem(0);
    }
    return QGraphicsItem::sceneEvent(event);
}

void ChatLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    ChatItem *item = mouseEventTargetItem(event->pos());
    if (item)
        item->mouseMoveEvent(event);
}

void ChatLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ChatItem *item = mouseEventTargetItem(event->pos());
    if (item)
        item->mousePressEvent(event);
}

void ChatLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    ChatItem *item = mouseEventTargetItem(event->pos());
    if (item)
        item->mouseReleaseEvent(event);
}

void ChatLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    ChatItem *item = mouseEventTargetItem(event->pos());
    if (item && !_hoverItem) {
        _hoverItem = item;
        item->hoverEnterEvent(event);
    }
}

void ChatLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (_hoverItem) {
        _hoverItem->hoverLeaveEvent(event);
        _hoverItem = 0;
    }
}

void ChatLine::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    ChatItem *item = mouseEventTargetItem(event->pos());
    if (item)
        item->hoverMoveEvent(event);
}

ChatItem *ChatLine::mouseEventTargetItem(const QPointF &pos)
{
    if (mouseGrabberItem())
        return mouseGrabberItem();
    return itemAt(pos);
}
