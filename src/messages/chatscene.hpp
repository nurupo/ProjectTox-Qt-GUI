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

#ifndef CHATSCENE_HPP
#define CHATSCENE_HPP

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QAbstractItemModel>
#include <QClipboard>
#include "messagemodel.hpp"

class QGraphicsSceneMouseEvent;
class ChatView;
class ChatLine;
class ChatItem;
class ColumnHandleItem;
class MarkerLineItem;

class ChatScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum CutoffMode {
        CutoffLeft,
        CutoffRight
    };

    enum ItemType {
        ChatLineType = QGraphicsItem::UserType + 1,
        ChatItemType,
        TimestampChatItemType,
        SenderChatItemType,
        ContentsChatItemType,
        ColumnHandleType,
        MarkerLineType
    };

    enum ClickMode {
        NoClick,
        DragStartClick,
        SingleClick,
        DoubleClick,
        TripleClick
    };

    ChatScene(QAbstractItemModel *model, qreal width, ChatView *parent);
    virtual ~ChatScene();

    inline QAbstractItemModel *model() const { return _model; }

    int rowByScenePos(qreal y) const;
    inline int rowByScenePos(const QPointF &pos) const { return rowByScenePos(pos.y()); }
    MessageModel::ColumnType columnByScenePos(qreal x) const;
    inline MessageModel::ColumnType columnByScenePos(const QPointF &pos) const { return columnByScenePos(pos.x()); }

    inline ChatView *chatView() const { return _chatView; }
    ChatItem *chatItemAt(const QPointF &pos) const;
    inline ChatLine *chatLine(int row) const { return (row < _lines.count()) ? _lines.value(row) : 0; }
    inline ChatLine *chatLine(const QModelIndex &index) const { return _lines.value(index.row()); }
    ChatLine *chatLine(MsgId msgId, bool matchExact = true, bool ignoreDayChange = true) const;

    inline ChatLine *lastLine() const { return _lines.count() ? _lines.last() : 0; }

    inline MarkerLineItem *markerLine() const { return _markerLine; }

    ColumnHandleItem *firstColumnHandle() const  { return _firstColHandle;  }
    ColumnHandleItem *secondColumnHandle() const { return _secondColHandle; }

    inline CutoffMode senderCutoffMode() const { return _cutoffMode; }
    inline void setSenderCutoffMode(CutoffMode mode) { _cutoffMode = mode; }

    QString selection() const;
    bool hasSelection() const;
    bool hasGlobalSelection() const { return _selectionStartRow >= 0; }
    bool isPosOverSelection(const QPointF &pos) const;
    bool isGloballySelecting() const { return _isSelecting; }
    void initiateDrag(QWidget *source);

    bool isScrollingAllowed() const;

signals:
    void lastLineChanged(QGraphicsItem *item, qreal offset);
    void layoutChanged(); // indicates changes to the scenerect due to resizing of the contentsitems
    void mouseMoveWhileSelecting(const QPointF &scenePos);

public slots:
    void updateForViewport(qreal width, qreal height);
    void setWidth(qreal width);
    void layout(int start, int end, qreal width);

    void setMarkerLineVisible(bool visible = true);
    void setMarkerLine(MsgId msgId = MsgId());
    void jumpToMarkerLine();

    // these are used by the chatitems to notify the scene and manage selections
    void setSelectingItem(ChatItem *item);
    ChatItem *selectingItem() const { return _selectingItem; }
    void startGlobalSelection(ChatItem *item, const QPointF &itemPos);
    void clearGlobalSelection();
    void clearSelection();
    void selectionToClipboard(QClipboard::Mode mode = QClipboard::Clipboard);
    void stringToClipboard(const QString &str_, QClipboard::Mode mode = QClipboard::Clipboard);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void handleClick(Qt::MouseButton button, const QPointF &scenePos);

protected slots:
    void rowsInserted(const QModelIndex &index, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void dataChanged(const QModelIndex &tl, const QModelIndex &br);

private slots:
    void firstHandlePositionChanged(qreal xpos);
    void secondHandlePositionChanged(qreal xpos);
    void secondHandlePositionChanged(qreal xpos, qreal sceneWidth);

    void rowsRemoved();
    void clickTimeout();

private:
    void setHandleXLimits();
    void updateSelection(const QPointF &pos);

    void updateSceneRect(qreal width);
    inline void updateSceneRect() { updateSceneRect(_sceneRect.width()); }
    void updateSceneRect(const QRectF &rect);

    ChatView *          _chatView;
    QAbstractItemModel *_model;
    QList<ChatLine *>   _lines;

    QRectF _sceneRect; // calls to QChatScene::sceneRect() are very expensive. As we manage the scenerect ourselves we store the size in a member variable.
    int    _firstLineRow; // the first row to display (aka: not a daychange msg)
    qreal  _viewportHeight;

    MarkerLineItem *_markerLine;
    bool _markerLineVisible;
    bool _markerLineValid;
    bool _markerLineJumpPending;

    ColumnHandleItem *_firstColHandle;
    ColumnHandleItem *_secondColHandle;
    qreal _firstColHandlePos;
    qreal _secondColHandlePos;
    CutoffMode _cutoffMode;

    ChatItem *_selectingItem;
    int  _selectionStartCol;
    int  _selectionMinCol;
    int  _selectionMaxCol;
    int  _selectionStartRow;
    int  _selectionEndRow;
    int  _firstSelectionRow;
    bool _isSelecting;

    QTimer _clickTimer;
    ClickMode _clickMode;
    QPointF _clickPos;
    bool _clickHandled;
    bool _leftButtonPressed;
};

#endif // CHATSCENE_HPP
