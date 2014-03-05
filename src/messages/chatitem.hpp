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

#ifndef CHATITEM_HPP
#define CHATITEM_HPP

#include <QAction>
#include <QObject>

#include "messagemodel.hpp"
#include "chatscene.hpp"
#include "clickable.hpp"
#include "smiley.hpp"

#include <QAbstractTextDocumentLayout>

class ChatLine;
class ChatView;
class ChatItemDocument;


/* All external positions are relative to the parent ChatLine */
/* Yes, that's also true for the boundingRect() and related things */

class ChatItem
{
protected:
    // boundingRect is relative to the parent ChatLine
    ChatItem(const QRectF &boundingRect, ChatLine *parent);
    virtual ~ChatItem();

public:
    const QAbstractItemModel *model() const;
    inline ChatLine *chatLine() const { return _parent; }
    ChatScene *chatScene() const;
    ChatView *chatView() const;
    int row() const;

    virtual MessageModel::ColumnType column() const = 0;

    // The boundingRect() is relative to the parent ChatLine
    inline QRectF boundingRect() const { return _boundingRect; }
    inline qreal width() const { return _boundingRect.width(); }
    inline qreal height() const { return _boundingRect.height(); }
    inline QPointF pos() const { return _boundingRect.topLeft(); }
    inline qreal x() const { return pos().x(); }
    inline qreal y() const { return pos().y(); }

    QPointF mapToLine(const QPointF &p) const { return p + pos(); }
    QPointF mapFromLine(const QPointF &p) const { return p - pos(); }
    QPointF mapToScene(const QPointF &p) const;
    QPointF mapFromScene(const QPointF &p) const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual inline int type() const { return ChatScene::ChatItemType; }

    QVariant data(int role) const;

    // selection stuff, to be called by the scene
    virtual QString selection() const;
    void clearSelection();
    void setFullSelection();
    void continueSelecting(const QPointF &pos);
    bool hasSelection() const;
    bool isPosOverSelection(const QPointF &pos) const;

    //QList<QRectF> findWords(const QString &searchWord, Qt::CaseSensitivity caseSensitive);

    virtual void addActionsToMenu(QMenu *menu, const QPointF &itemPos);
    virtual void handleClick(const QPointF &pos, ChatScene::ClickMode clickMode);

    //! Remove internally cached data
    //! This removes the cached QTextDocument to avoid wasting space for nonvisible ChatLines.
    virtual void clearCache();

protected:
    enum SelectionMode {
        NoSelection,
        PartialSelection,
        FullSelection
    };

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) {}
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) {}
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *) {}

    QAbstractTextDocumentLayout::Selection selectionLayout() const;
    QTextDocument *document() const;
    virtual void initDocument(QTextDocument *doc);

    inline qint16 selectionStart() const { return _selectionStart; }
    inline void setSelectionStart(qint16 start) { _selectionStart = start; }
    inline qint16 selectionEnd() const { return _selectionEnd; }
    inline void setSelectionEnd(qint16 end) { _selectionEnd = end; }
    inline SelectionMode selectionMode() const { return _selectionMode; }
    inline void setSelectionMode(SelectionMode mode) { _selectionMode = mode; }
    void setSelection(SelectionMode mode, qint16 start, qint16 end);

    qint16 posToCursor(const QPointF &posInLine) const;

    void setGeometry(qreal width, qreal height);
    inline void setHeight(const qreal &height) { /*clearCache();*/ _boundingRect.setHeight(height); }
    void setWidth(const qreal &width);
    inline void setPos(const QPointF &pos) {_boundingRect.moveTopLeft(pos); }

private:
    ChatLine *_parent;
    QRectF _boundingRect;

    SelectionMode _selectionMode;
    int _selectionStart;
    int _selectionEnd;

    mutable ChatItemDocument *mDoc;

    friend class ChatLine;
    friend class ChatItemDocument;
};

class ChatItemDocument {
public:
    ChatItemDocument(ChatItem *parent) : chatItem(parent) {}
    void callInitDocument() { chatItem->initDocument(&doc); }
    QTextDocument doc;

private:
    ChatItem *chatItem;
};

// ************************************************************
// TimestampChatItem
// ************************************************************

//! A ChatItem for the timestamp column
class TimestampChatItem : public ChatItem
{
public:
    TimestampChatItem(const QRectF &boundingRect, ChatLine *parent);
    virtual inline int type() const { return ChatScene::TimestampChatItemType; }
    virtual inline MessageModel::ColumnType column() const { return MessageModel::TimestampColumn; }

protected:
    virtual void initDocument(QTextDocument *doc);
};

// ************************************************************
// SenderChatItem
// ************************************************************
//! A ChatItem for the sender column
class SenderChatItem : public ChatItem
{
public:
    SenderChatItem(const QRectF &boundingRect, ChatLine *parent);
    virtual inline int type() const { return ChatScene::SenderChatItemType; }
    virtual inline MessageModel::ColumnType column() const { return MessageModel::SenderColumn; }

protected:
    virtual void initDocument(QTextDocument *doc);
};

// ************************************************************
// ContentsChatItem
// ************************************************************
class ContentsChatItemPrivate;

//! A ChatItem for the contents column
class ContentsChatItem : public ChatItem
{
    Q_DECLARE_TR_FUNCTIONS(ContentsChatItem)

public:
    ContentsChatItem(const QPointF &pos, const qreal &width, ChatLine *parent);
    ~ContentsChatItem();

    void clearCache();

    virtual inline int type() const { return ChatScene::ContentsChatItemType; }
    inline MessageModel::ColumnType column() const { return MessageModel::ContentsColumn; }
    virtual QString selection() const;

protected:
    virtual void initDocument(QTextDocument *doc);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void handleClick(const QPointF &pos, ChatScene::ClickMode clickMode);

    virtual void addActionsToMenu(QMenu *menu, const QPointF &pos);
    virtual void copyLinkToClipboard();

private:
    class ActionProxy;

    Clickable clickableAt(const QPointF &pos) const;

    void endHoverMode();

    qreal setGeometryByWidth(qreal w);

    // we need a receiver for Action signals
    static ActionProxy mActionProxy;

    ContentsChatItemPrivate *privateData() const;
    mutable ContentsChatItemPrivate *_data;

    friend class ChatLine;
    friend class ContentsChatItemPrivate;
};

class ContentsChatItemPrivate {
public:
    ContentsChatItem *contentsItem;
    ClickableList clickables;
    Clickable currentClickable;
    Clickable activeClickable;

    SmileyList smileys;

    ContentsChatItemPrivate(ContentsChatItem *parent) : contentsItem(parent) {}
};

//! Acts as a proxy for Action signals targetted at a ContentsChatItem
/** Since a ChatItem is not a QObject, hence cannot receive signals, we use a static ActionProxy
 *  as a receiver instead. This avoids having to handle ChatItem actions (e.g. context menu entries)
 *  outside the ChatItem.
 */
class ContentsChatItem::ActionProxy : public QObject
{
    Q_OBJECT

public slots:
    inline void copyLinkToClipboard() { item()->copyLinkToClipboard(); }

private:
    /// Returns the ContentsChatItem that should receive the action event.
    /** For efficiency reasons, values are not checked for validity. You gotta make sure that you set the data() member
     *  in the Action correctly.
     *  @return The ChatItem from which the sending Action originated
     */
    inline ContentsChatItem *item() const
    {
        return static_cast<ContentsChatItem *>(qobject_cast<QAction *>(sender())->data().value<void *>());
    }
};

#endif // CHATITEM_HPP
