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

#include "chatview.hpp"
#include "chatscene.hpp"
#include "chatline.hpp"
#include <QAbstractSlider>
#include <QScrollBar>
#include <QApplication>
#include "Settings/settings.hpp"
#include "messagefilter.hpp"
#include <QMenu>

ChatView::ChatView(MessageFilter *model, QWidget *parent) :
    QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setAlignment(Qt::AlignLeft|Qt::AlignBottom);
    setInteractive(true);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    setBackgroundBrush(QApplication::palette().window());
    setFrameShape(QFrame::NoFrame);

    _scrollTimer.setInterval(100);
    _scrollTimer.setSingleShot(true);
    connect(&_scrollTimer, SIGNAL(timeout()), SLOT(scrollTimerTimeout()));

    _scene = new ChatScene(model, viewport()->width(), this);
    connect(_scene, SIGNAL(sceneRectChanged(const QRectF &)), this, SLOT(adjustSceneRect()));
    connect(_scene, SIGNAL(lastLineChanged(QGraphicsItem *, qreal)), this, SLOT(lastLineChanged(QGraphicsItem *, qreal)));
    connect(_scene, SIGNAL(mouseMoveWhileSelecting(const QPointF &)), this, SLOT(mouseMoveWhileSelecting(const QPointF &)));
    setScene(_scene);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScrollbarChanged(int)));
    _lastScrollbarPos = verticalScrollBar()->value();

    // Update timstamps
    connect(&Settings::getInstance(), &Settings::timestampFormatChanged, this, &ChatView::clearCache);

    // Actions
    hidePlain = new QAction(tr("Text messages"), this);
    hidePlain->setCheckable(true);
    connect(hidePlain, &QAction::toggled, model, &MessageFilter::filterPlain);

    hideAction = new QAction(tr("Actions"), this);
    hideAction->setCheckable(true);
    connect(hideAction, &QAction::toggled, model, &MessageFilter::filterAction);

    hideNick = new QAction(tr("Name changes"), this);
    hideNick->setCheckable(true);
    connect(hideNick, &QAction::toggled, model, &MessageFilter::filterNick);

    hideError = new QAction(tr("Errors"), this);
    hideError->setCheckable(true);
    connect(hideError, &QAction::toggled, model, &MessageFilter::filterError);

    hideDaychange = new QAction(tr("Day changes"), this);
    hideDaychange->setCheckable(true);
    connect(hideDaychange, &QAction::toggled, model, &MessageFilter::filterDaychange);
}

MsgId ChatView::lastMsgId() const
{
    if (!scene())
        return MsgId();

    QAbstractItemModel *model = scene()->model();
    if (!model || model->rowCount() == 0)
        return MsgId();

    return model->index(model->rowCount() - 1, 0).data(MessageModel::MsgIdRole).value<MsgId>();
}

MsgId ChatView::lastVisibleMsgId() const
{
    ChatLine *line = lastVisibleChatLine();

    if (line)
        return line->msgId();

    return MsgId();
}

bool chatLinePtrLessThan(ChatLine *one, ChatLine *other)
{
    return one->row() < other->row();
}

// TODO: figure out if it's cheaper to use a cached list (that we'd need to keep updated)
QSet<ChatLine *> ChatView::visibleChatLines(Qt::ItemSelectionMode mode) const
{
    QSet<ChatLine *> result;
    foreach(QGraphicsItem *item, items(viewport()->rect().adjusted(-1, -1, 1, 1), mode)) {
        ChatLine *line = qgraphicsitem_cast<ChatLine *>(item);
        if (line)
            result.insert(line);
    }
    return result;
}

QList<ChatLine *> ChatView::visibleChatLinesSorted(Qt::ItemSelectionMode mode) const
{
    QList<ChatLine *> result = visibleChatLines(mode).toList();
    qSort(result.begin(), result.end(), chatLinePtrLessThan);
    return result;
}

ChatLine *ChatView::lastVisibleChatLine(bool ignoreDayChange) const
{
    if (!scene())
        return 0;

    QAbstractItemModel *model = scene()->model();
    if (!model || model->rowCount() == 0)
        return 0;

    int row = -1;

    QSet<ChatLine *> visibleLines = visibleChatLines(Qt::ContainsItemBoundingRect);
    foreach(ChatLine *line, visibleLines) {
        if (line->row() > row && (ignoreDayChange ? line->msgType() != Message::DayChange : true))
            row = line->row();
    }

    if (row >= 0)
        return scene()->chatLine(row);

    return 0;
}

void ChatView::addActionsToMenu(QMenu *menu, const QPointF &pos)
{
    Q_UNUSED(pos)

    QMenu *filterMenu =  new QMenu(tr("Hide messages"), menu);
    filterMenu->setIcon(QIcon(":/icons/filter.png"));
    filterMenu->addAction(hidePlain);
    filterMenu->addAction(hideAction);
    filterMenu->addAction(hideNick);
    filterMenu->addAction(hideError);
    filterMenu->addAction(hideDaychange);


    menu->addMenu(filterMenu);
}

void ChatView::setHasCache(ChatLine *line, bool hasCache)
{
    if (hasCache)
        _linesWithCache.insert(line);
    else
        _linesWithCache.remove(line);
}

void ChatView::clearCache()
{
    QSet<ChatLine *>::iterator iter = _linesWithCache.begin();
    while (iter != _linesWithCache.end()) {
        ChatLine *line = *iter;
        line->clearCache();
        iter = _linesWithCache.erase(iter);
    }
    update();
}

void ChatView::setMarkerLineVisible(bool visible)
{
    scene()->setMarkerLineVisible(visible);
}

void ChatView::setMarkerLine(MsgId msgId)
{
    Q_UNUSED(msgId)
    // TODO MKO Markerline
}

void ChatView::jumpToMarkerLine()
{
    scene()->jumpToMarkerLine();
}

bool ChatView::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            if (!verticalScrollBar()->isVisible()) {
                return true;
            }
        default:
            break;
        }

        // enable ctrl+c even when contextmenu is hidden
        if (keyEvent == QKeySequence::Copy) {
            scene()->selectionToClipboard();
            return true;
        }
    }

    if (event->type() == QEvent::Wheel) {
        if (!verticalScrollBar()->isVisible()) {
            return true;
        }
    }

    return QGraphicsView::event(event);
}

void ChatView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    // FIXME: do we really need to scroll down on resize?

    // we can reduce viewport updates if we scroll to the bottom allready at the beginning
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    scene()->updateForViewport(viewport()->width(), viewport()->height());
    adjustSceneRect();

    _lastScrollbarPos = verticalScrollBar()->maximum();
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());

    checkChatLineCaches();
}

void ChatView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    checkChatLineCaches();
}

void ChatView::verticalScrollbarChanged(int newPos)
{
    QAbstractSlider *vbar = verticalScrollBar();
    Q_ASSERT(vbar);

    // check for backlog request
    if (newPos < _lastScrollbarPos) {
        int relativePos = 100;
        if (vbar->maximum() - vbar->minimum() != 0)
            relativePos = (newPos - vbar->minimum()) * 100 / (vbar->maximum() - vbar->minimum());

        if (relativePos < 20) {
            // TODO MKO scene()->requestBacklog();
        }
    }
    _lastScrollbarPos = newPos;

    // FIXME: Fugly workaround for the ChatView scrolling up 1px on buffer switch
    if (vbar->maximum() - newPos <= 2)
        vbar->setValue(vbar->maximum());
}

void ChatView::lastLineChanged(QGraphicsItem *chatLine, qreal offset)
{
    Q_UNUSED(chatLine)
    // disabled until further testing/discussion
    //if(!scene()->isScrollingAllowed())
    //  return;

    QAbstractSlider *vbar = verticalScrollBar();
    Q_ASSERT(vbar);
    if (vbar->maximum() - vbar->value() <= (offset + 5)) { // 5px grace area
        vbar->setValue(vbar->maximum());
    }
}

void ChatView::adjustSceneRect()
{
    // Workaround for QTBUG-6322
    // If the viewport's sceneRect() is (almost) as wide as as the viewport itself,
    // Qt wants to reserve space for scrollbars even if they're turned off, resulting in
    // an ugly white space at the bottom of the ChatView.
    // Since the view's scene's width actually doesn't matter at all, we just adjust it
    // by some hopefully large enough value to avoid this problem.

    setSceneRect(scene()->sceneRect().adjusted(0, 0, -25, 0));
}

void ChatView::checkChatLineCaches()
{
    qreal top = mapToScene(viewport()->rect().topLeft()).y() - 10; // some grace area to avoid premature cleaning
    qreal bottom = mapToScene(viewport()->rect().bottomRight()).y() + 10;
    QSet<ChatLine *>::iterator iter = _linesWithCache.begin();
    while (iter != _linesWithCache.end()) {
        ChatLine *line = *iter;
        if (line->pos().y() + line->height() < top || line->pos().y() > bottom) {
            line->clearCache();
            iter = _linesWithCache.erase(iter);
        }
        else
            ++iter;
    }
}

void ChatView::mouseMoveWhileSelecting(const QPointF &scenePos)
{
    int y = (int)mapFromScene(scenePos).y();
    _scrollOffset = 0;
    if (y < 0)
        _scrollOffset = y;
    else if (y > height())
        _scrollOffset = y - height();

    if (_scrollOffset && !_scrollTimer.isActive())
        _scrollTimer.start();
}

void ChatView::scrollTimerTimeout()
{
    // scroll view
    QAbstractSlider *vbar = verticalScrollBar();
    if (_scrollOffset < 0 && vbar->value() > 0)
        vbar->setValue(qMax(vbar->value() + _scrollOffset, 0));
    else if (_scrollOffset > 0 && vbar->value() < vbar->maximum())
        vbar->setValue(qMin(vbar->value() + _scrollOffset, vbar->maximum()));
}

void ChatView::markerLineSet(MsgId msg)
{
    scene()->setMarkerLine(msg);
    scene()->setMarkerLineVisible(true);
}

