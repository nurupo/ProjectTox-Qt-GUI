#ifndef CHATLINE_HPP
#define CHATLINE_HPP

#include <QGraphicsItem>

#include "messagemodel.hpp"
#include "chatitem.hpp"
#include "chatscene.hpp"

class ChatLine : public QGraphicsItem
{
public:
    ChatLine(int row, QAbstractItemModel *model,
             const qreal &width,
             const qreal &firstWidth, const qreal &secondWidth, const qreal &thirdWidth,
             const QPointF &secondPos, const QPointF &thirdPos,
             QGraphicsItem *parent = 0);
    virtual ~ChatLine();

    virtual inline QRectF boundingRect() const { return QRectF(0, 0, _width, _height); }
    inline QModelIndex index() const { return model()->index(row(), 0); }
    inline MsgId msgId() const { return index().data(MessageModel::MsgIdRole).value<MsgId>(); }
    inline Message::Type msgType() const { return (Message::Type)index().data(MessageModel::TypeRole).toInt(); }

    inline int row() const { return _row; }
    inline void setRow(int row) { _row = row; }

    inline const QAbstractItemModel *model() const { return _model; }
    inline ChatScene *chatScene() const { return qobject_cast<ChatScene *>(scene()); }
    inline ChatView *chatView() const { return chatScene() ? chatScene()->chatView() : 0; }

    inline qreal width() const { return _width; }
    inline qreal height() const { return _height; }

    ChatItem *item(MessageModel::ColumnType column);
    ChatItem *itemAt(const QPointF &pos);
    inline ChatItem *timestampItem() { return &_timestampItem; }
    inline ChatItem *senderItem() { return &_senderItem; }
    inline ContentsChatItem *contentsItem() { return &_contentsItem; }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    enum { Type = ChatScene::ChatLineType };
    virtual inline int type() const { return Type; }

    // pos is relative to the parent ChatLine
    void setFirstColumn(const qreal &firstWidth, const qreal &secondwidth, const QPointF &secondPos);
    // setSecondColumn and setGeometryByWidth both also relocate the chatline.
    // the _bottom_ position is passed via linePos. linePos is updated to the top of the chatLine.
    void setSecondColumn(const qreal &secondWidth, const qreal &thirdWidth, const QPointF &thirdPos, qreal &linePos);
    void setGeometryByWidth(const qreal &width, const qreal &thirdWidth, qreal &linePos);

    void setSelected(bool selected, MessageModel::ColumnType minColumn = MessageModel::ContentsColumn);
    void setHighlighted(bool highlighted);

    void clearCache();

protected:
    virtual bool sceneEvent(QEvent *event);

    // These need to be relayed to the appropriate ChatItem
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    ChatItem *mouseEventTargetItem(const QPointF &pos);

    inline ChatItem *mouseGrabberItem() const { return _mouseGrabberItem; }
    void setMouseGrabberItem(ChatItem *item)  { _mouseGrabberItem = item; }

private:
    int _row;
    QAbstractItemModel *_model;
    ContentsChatItem  _contentsItem;
    SenderChatItem    _senderItem;
    TimestampChatItem _timestampItem;
    qreal _width;
    qreal _height;

    enum { ItemMask = 0x3f,
           Selected = 0x40,
           Highlighted = 0x80 };
    // _selection[1..0] ... Min Selected Column (See MessageModel::ColumnType)
    // _selection[5..2] ... reserved for new column types
    // _selection[6] ...... Selected
    // _selection[7] ...... Highlighted
    quint8 _selection; // save space, so we put both the col and the flags into one byte

    ChatItem *_mouseGrabberItem;
    ChatItem *_hoverItem;
};

#endif // CHATLINE_HPP
