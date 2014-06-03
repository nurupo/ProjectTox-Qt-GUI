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

#include "chatitem.hpp"
#include "chatline.hpp"
#include "chatview.hpp"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QMenu>
#include <QFontMetricsF>
#include <QApplication>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include "smileytextobject.hpp"
#include <QStyleOption>
#include <QTextDocumentFragment>

ChatItem::ChatItem(const QRectF &boundingRect, ChatLine *parent) :
    _parent(parent),
    _boundingRect(boundingRect),
    _selectionMode(NoSelection),
    _selectionStart(-1),
    mDoc(nullptr)
{
}

ChatItem::~ChatItem()
{
    if(mDoc)
        delete mDoc;

    qDeleteAll(mHighlights);
}

const QAbstractItemModel *ChatItem::model() const
{
    return chatLine()->model();
}

ChatScene *ChatItem::chatScene() const
{
    return chatLine()->chatScene();
}

ChatView *ChatItem::chatView() const
{
    return chatScene()->chatView();
}

int ChatItem::row() const
{
    return chatLine()->row();
}

QPointF ChatItem::mapToScene(const QPointF &p) const
{
    return chatLine()->mapToScene(p /* + pos() */);
}

QPointF ChatItem::mapFromScene(const QPointF &p) const
{
    return chatLine()->mapFromScene(p) /* - pos() */;
}

void ChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette = QApplication::palette();
    ctx.palette.setBrush(QPalette::Active, QPalette::Text, data(MessageModel::ForegroundRole).value<QBrush>());
    QPalette::ColorGroup cg = chatView()->hasFocus() ? QPalette::Active : QPalette::Inactive;

    //qDebug() << "-A--------";
    // Highlights (search results)
    for (Highlight *h : mHighlights) {
        //qDebug() << h->start() << h->length() << h->type();
        QTextCursor c(document());
        c.setPosition(h->start());
        c.setPosition(h->start() + h->length(), QTextCursor::KeepAnchor);

        QTextCharFormat f;
        if(h->type() == Highlight::Current)
            f.setBackground(QBrush("#a40000"));
        else
            f.setBackground(QBrush("#C4A000"));
        f.setForeground(Qt::white);

        QAbstractTextDocumentLayout::Selection highlight;
        highlight.cursor = c;
        highlight.format = f;
        ctx.selections.append(highlight);
    }
    //qDebug() << "-E--------";

    // Selection
    QAbstractTextDocumentLayout::Selection selection = selectionLayout();
    selection.format.setBackground(ctx.palette.brush(cg, QPalette::Highlight));
    selection.format.setForeground(ctx.palette.brush(cg, QPalette::HighlightedText));
    ctx.selections.append(selection);

    painter->translate(pos());
    document()->documentLayout()->draw(painter, ctx);

    painter->restore();
}

QVariant ChatItem::data(int role) const
{
    QModelIndex index = model()->index(row(), column());
    if (!index.isValid()) {
        qWarning() << "ChatItem::data(): model index is invalid!" << index;
        return QVariant();
    }
    return model()->data(index, role);
}

QString ChatItem::selection() const
{
    if (selectionMode() == FullSelection)
        return data(MessageModel::DisplayRole).toString();
    if (selectionMode() == PartialSelection) {
        int start = qMin(selectionStart(), selectionEnd());
        int end   = start + qAbs(selectionStart() - selectionEnd());

        QTextCursor cSelect(document());
        cSelect.setPosition(start);
        cSelect.setPosition(end, QTextCursor::KeepAnchor);
        return cSelect.selectedText();
    }
    return QString();
}

void ChatItem::clearSelection()
{
    if (_selectionMode != NoSelection) {
        _selectionMode = NoSelection;
        chatLine()->update();
    }
}

void ChatItem::setFullSelection()
{
    if (_selectionMode != FullSelection) {
        _selectionMode = FullSelection;
        chatLine()->update();
    }
}

void ChatItem::continueSelecting(const QPointF &pos)
{
    _selectionMode = PartialSelection;
    _selectionEnd = posToCursor(pos);
    chatLine()->update();
}

bool ChatItem::hasSelection() const
{
    if (_selectionMode == NoSelection)
        return false;
    if (_selectionMode == FullSelection)
        return true;
    // partial
    return _selectionStart != _selectionEnd;
}

bool ChatItem::isPosOverSelection(const QPointF &pos) const
{
    if (_selectionMode == FullSelection)
        return true;
    if (_selectionMode == PartialSelection) {
        int cursor = posToCursor(pos);
        return cursor >= qMin(_selectionStart, _selectionEnd) && cursor <= qMax(_selectionStart, _selectionEnd);
    }
    return false;
}

Highlight *ChatItem::addHighlight(int start, int length)
{
     Highlight *h = new Highlight(Highlight::Found, this, start,length);
     mHighlights << h;
     chatLine()->update();

    return h;
}

void ChatItem::highlightRemove(Highlight *h)
{
    int pos = mHighlights.indexOf(h);
    if (mHighlights.count() == 0 || pos < 0) {
        return;
    }

    if(h) {
        delete h;
        h = nullptr;
    }
    mHighlights.removeAt(pos);
    chatLine()->update();
}

void ChatItem::addActionsToMenu(QMenu *menu, const QPointF &itemPos)
{
    Q_UNUSED(menu)
    Q_UNUSED(itemPos)
}

void ChatItem::handleClick(const QPointF &pos, ChatScene::ClickMode clickMode)
{
    // single clicks are already handled by the scene (for clearing the selection)
    if (clickMode == ChatScene::DragStartClick) {
        chatScene()->setSelectingItem(this);
        _selectionStart = _selectionEnd = posToCursor(pos);
        _selectionMode = NoSelection; // will be set to PartialSelection by mouseMoveEvent
        chatLine()->update();
    }
}

void ChatItem::clearCache()
{
    if(mDoc) {
        delete mDoc;
        mDoc = NULL;
    }
}

void ChatItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        if (boundingRect().contains(event->pos())) {
            qint16 end = posToCursor(event->pos());
            if (end != _selectionEnd) {
                _selectionEnd = end;
                _selectionMode = (_selectionStart != _selectionEnd ? PartialSelection : NoSelection);
                chatLine()->update();
            }
        }
        else {
            setFullSelection();
            chatScene()->startGlobalSelection(this, event->pos());
        }
        event->accept();
    }
    else {
        event->ignore();
    }
}

void ChatItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        event->accept();
    else
        event->ignore();
}

void ChatItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (_selectionMode != NoSelection && event->button() == Qt::LeftButton) {
        chatScene()->selectionToClipboard(QClipboard::Selection);
        event->accept();
    }
    else
        event->ignore();
}

QAbstractTextDocumentLayout::Selection ChatItem::selectionLayout() const
{
        QAbstractTextDocumentLayout::Selection selection;

        if (!hasSelection())
            return selection;

        int start;
        int end;

        if (selectionMode() == FullSelection) {
            start = 0;
            end = document()->characterCount()-1;
        }
        else {
            start = selectionStart();
            end   = selectionEnd();
        }

        QTextCursor c(document());
        c.setPosition(start);
        c.setPosition(end, QTextCursor::KeepAnchor);
        selection.cursor = c;

        return selection;
}

QTextDocument *ChatItem::document() const
{
    if (mDoc)
        return &(mDoc->doc);

    ChatItem *that = const_cast<ChatItem *>(this);
    mDoc = new ChatItemDocument(that);
    mDoc->callInitDocument();

    if(chatScene())
        chatView()->setHasCache(chatLine(), true);

    return &(mDoc->doc);
}

void ChatItem::initDocument(QTextDocument *doc)
{
    doc->setPlainText(data(MessageModel::DisplayRole).toString());
    doc->setTextWidth(width());
}

void ChatItem::setSelection(ChatItem::SelectionMode mode, qint16 start, qint16 end)
{
    _selectionMode = mode;
    _selectionStart = start;
    _selectionEnd = end;
    chatLine()->update();
}

qint16 ChatItem::posToCursor(const QPointF &posInLine) const
{
    QPointF pos = mapFromLine(posInLine);
    if (pos.y() > height())
        return document()->characterCount()-1;
    if (pos.y() < 0)
        return 0;

    int found = document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);

    if(found < 0)
        return 0;

    return found;
}

void ChatItem::setGeometry(qreal width, qreal height)
{
    _boundingRect.setSize(QSizeF(width, height));
    document()->setTextWidth(width);
}

void ChatItem::setWidth(const qreal &width)
{
    _boundingRect.setWidth(width);
    document()->setTextWidth(width);
}



// ************************************************************
// SenderChatItem
// ************************************************************

SenderChatItem::SenderChatItem(const QRectF &boundingRect, ChatLine *parent) :
    ChatItem(boundingRect, parent)
{
}

void SenderChatItem::initDocument(QTextDocument *doc)
{
    // Hide double sender names
    QModelIndex lastIndex = model()->index(row()-1, column());
    if (lastIndex.isValid()
            && data(MessageModel::TypeRole).toInt() == Message::Plain
            && model()->data(lastIndex, MessageModel::TypeRole).toInt() == Message::Plain
            && (data(MessageModel::FlagsRole).toInt() & Message::Self) == (model()->data(lastIndex, MessageModel::FlagsRole).toInt() & Message::Self)) {
        doc->setPlainText("");
    }
    else {
        doc->setPlainText(data(MessageModel::DisplayRole).toString());
    }

    doc->setTextWidth(width());

    QTextOption o;
    o.setWrapMode(QTextOption::NoWrap);
    doc->setDefaultTextOption(o);
}

// ************************************************************
// ContentsChatItem
// ************************************************************

ContentsChatItem::ActionProxy ContentsChatItem::mActionProxy;

ContentsChatItem::ContentsChatItem(const QPointF &pos, const qreal &width, ChatLine *parent) :
    ChatItem(QRectF(pos, QSizeF(width, 0)), parent),
    _data(NULL)
{
    setPos(pos);
    setGeometryByWidth(width);
}

ContentsChatItem::~ContentsChatItem()
{
    if(_data)
        delete _data;
}

void ContentsChatItem::clearCache()
{
    ChatItem::clearCache();

    if(_data) {
        delete _data;
        _data = NULL;
    }
}

QString ContentsChatItem::selection() const
{
    if (selectionMode() == FullSelection)
        return data(MessageModel::DisplayRole).toString();
    if (selectionMode() == PartialSelection) {
        int start = qMin(selectionStart(), selectionEnd());
        int end   = start + qAbs(selectionStart() - selectionEnd());
        int endOffset = 0;
        int startOffset = 0;

        for (const Smiley &s : privateData()->smileys) {
            if (start > s.smileyfiedStart()) {
                startOffset += s.text().count() - ((s.type() == Smiley::Emoji) ? s.graphics().count() : 1);
            }
            if (s.smileyfiedStart() >= start && s.smileyfiedStart()+1 <= end) {
                endOffset += s.text().count() - ((s.type() == Smiley::Emoji) ? s.graphics().count() : 1);
            }
        }

        return data(MessageModel::DisplayRole).toString().mid(start+startOffset, end-start+endOffset);
    }
    return QString();
}

const SmileyList *ContentsChatItem::smileyList() const
{
    return &(privateData()->smileys);
}

void ContentsChatItem::initDocument(QTextDocument *doc)
{
    QString text = data(MessageModel::DisplayRole).toString();

    privateData()->smileys = SmileyList::fromText(text);

    doc->setPlainText(text);
    doc->setTextWidth(width());

    // Replace smileys
    QTextCursor c(doc);
    int i=0;
    for(const Smiley &smiley : privateData()->smileys) {

        // Remove textual smiley representation
        c.setPosition(smiley.smileyfiedStart());
        c.setPosition(smiley.smileyfiedStart()+smiley.text().count(), QTextCursor::KeepAnchor);
        c.removeSelectedText();

        if (smiley.type() == Smiley::Pixmap) {

            QObject *smileyInterface = new SmileyTextObject(smiley.graphics());
            doc->documentLayout()->registerHandler(QTextFormat::UserObject + i+1, smileyInterface);

            QTextCharFormat smileyFormat;
            smileyFormat.setObjectType(QTextFormat::UserObject + i+1);
            c.insertText(QString(QChar::ObjectReplacementCharacter), smileyFormat);
            i++;
        }
        else if (smiley.type() == Smiley::Emoji) {

            c.insertText(smiley.graphics());
            c.setPosition(smiley.smileyfiedStart(), QTextCursor::KeepAnchor);
            QTextCharFormat format;
            format.setFont(smiley.emojiFont());
            c.mergeCharFormat(format);
        }
    }

    // Clickables
    QString dummtext = text;
    for(const Smiley &smiley : privateData()->smileys) {
        QString dummy = "#";
        if(smiley.type() == Smiley::Emoji) {
            for(int i=1; i<smiley.graphics().count(); i++)
                dummy += "#";
        }
        dummtext.replace(smiley.smileyfiedStart(), smiley.text().count(), dummy);
    }

    privateData()->clickables = ClickableList::fromString(dummtext);
    foreach(Clickable click, privateData()->clickables) {
        QTextCursor c(doc);
        c.setPosition(click.start());
        c.setPosition(click.start()+click.length(), QTextCursor::KeepAnchor);
        QTextCharFormat f;
        f.setAnchor(true);
        f.setAnchorHref(c.selectedText());
        f.setForeground(QApplication::palette().brush(QPalette::Link));
        c.mergeCharFormat(f);
    }
}

void ContentsChatItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // mouse move events always mean we're not hovering anymore...
    endHoverMode();
    ChatItem::mouseMoveEvent(event);
}

void ContentsChatItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    endHoverMode();
    event->accept();
}

void ContentsChatItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    bool onClickable = false;
    Clickable click = clickableAt(event->pos());
    if (click.isValid()) {
        if (click.type() == Clickable::Url) {
            onClickable = true;
        }
        if (onClickable) {
            privateData()->currentClickable = click;

            // Set hand cursor
            chatLine()->setCursor(Qt::PointingHandCursor);

            // Underline clickable
            QTextCursor c(document());
            c.setPosition(click.start());
            c.setPosition(click.start()+click.length(), QTextCursor::KeepAnchor);
            QTextCharFormat f;
            f.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            c.mergeCharFormat(f);

            chatLine()->update();
            return;
        }
    }
    if (!onClickable)
        endHoverMode();

    event->accept();
}

void ContentsChatItem::handleClick(const QPointF &pos, ChatScene::ClickMode clickMode)
{
    if (clickMode == ChatScene::SingleClick) {
        qint16 idx = posToCursor(pos);
        Clickable foo = privateData()->clickables.atCursorPos(idx);
        if (foo.isValid()) {
            QTextCursor c(document());
            c.setPosition(idx +1); // TODO MKO +1? Bug in link detection?
            foo.activate(c.charFormat().anchorHref());
        }
    }
    else if (clickMode == ChatScene::DoubleClick) {
        chatScene()->setSelectingItem(this);
        setSelectionMode(PartialSelection);
        Clickable click = clickableAt(pos);
        if (click.isValid()) {
            setSelectionStart(click.start());
            setSelectionEnd(click.start() + click.length());
        }
        else {
            // find word boundary
            qint16 cursor = posToCursor(pos);
            QTextCursor c(document());
            c.setPosition(cursor);

            c.select(QTextCursor::WordUnderCursor);
            setSelectionStart(c.selectionStart()); // TODO MKO write SelectionLayout directly?
            setSelectionEnd(c.selectionEnd());
        }
        chatLine()->update();
    }
    else if (clickMode == ChatScene::TripleClick) {
        QTextCursor c(document());
        c.select(QTextCursor::Document);
        setSelection(PartialSelection, 0, c.selectionEnd());
    }
    ChatItem::handleClick(pos, clickMode);
}

void ContentsChatItem::addActionsToMenu(QMenu *menu, const QPointF &pos)
{
    if (privateData()->currentClickable.isValid()) {
        Clickable click = privateData()->currentClickable;
        switch (click.type()) {
        case Clickable::Url:
            privateData()->activeClickable = click;
            menu->addAction(QIcon(":/icons/world_link.png"), tr("Copy Link Address"), &mActionProxy, SLOT(copyLinkToClipboard()))->setData(QVariant::fromValue<void *>(this));
            break;
        default:
            break;
        }
    }
    else {
        // Buffer-specific actions
        ChatItem::addActionsToMenu(menu, pos);
    }
}

void ContentsChatItem::copyLinkToClipboard()
{
    Clickable click = privateData()->activeClickable;

    if (click.isValid() && click.type() == Clickable::Url) {
        QTextCursor c(document());
        c.setPosition(click.start()+1); // TODO MKO +1? Bug in link detection?
        chatScene()->stringToClipboard(c.charFormat().anchorHref());
    }
}

Clickable ContentsChatItem::clickableAt(const QPointF &pos) const
{
    return privateData()->clickables.atCursorPos(posToCursor(pos));
}

void ContentsChatItem::endHoverMode()
{
    if (privateData()) {
        if (privateData()->currentClickable.isValid()) {
            privateData()->currentClickable = Clickable();

            // Unset hand cursor
            chatLine()->unsetCursor();

            // De-underline clickable
            QTextCursor c(document());
            c.setPosition(0);
            c.setPosition(document()->characterCount()-1, QTextCursor::KeepAnchor);
            QTextCharFormat f;
            f.setUnderlineStyle(QTextCharFormat::NoUnderline);
            c.mergeCharFormat(f);
        }
        chatLine()->update();
    }
}

qreal ContentsChatItem::setGeometryByWidth(qreal w)
{
    document()->setTextWidth(w);
    qreal h = document()->size().height();

    if (w != width() || h != height())
        setGeometry(w, h);

    return h;

}

ContentsChatItemPrivate *ContentsChatItem::privateData() const
{
    if (!_data) {
        ContentsChatItem *that = const_cast<ContentsChatItem *>(this);
        that->_data = new ContentsChatItemPrivate(that);
    }
    return _data;
}

// ************************************************************
// TimestampChatItem
// ************************************************************


TimestampChatItem::TimestampChatItem(const QRectF &boundingRect, ChatLine *parent) :
    ChatItem(boundingRect, parent)
{
}

void TimestampChatItem::initDocument(QTextDocument *doc)
{
    ChatItem::initDocument(doc);

    QTextOption o;
    o.setAlignment(Qt::AlignRight);
    o.setWrapMode(QTextOption::NoWrap);
    doc->setDefaultTextOption(o);
}
