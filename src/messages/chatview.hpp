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

#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QGraphicsView>
#include "id.hpp"
#include "messagemodel.hpp"

class ChatLine;
class ChatScene;

class ChatView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ChatView(MessageModel *model, QWidget *parent = 0);

    virtual MsgId lastMsgId() const;
    virtual MsgId lastVisibleMsgId() const;
    inline ChatScene *scene() const { return _scene; }

    //! Return a set of ChatLines currently visible in the view
    /** \param mode How partially visible ChatLines are handled
     *  \return A set of visible ChatLines */
    QSet<ChatLine *> visibleChatLines(Qt::ItemSelectionMode mode = Qt::ContainsItemBoundingRect) const;

    //! Return a sorted list of ChatLines currently visible in the view
    /** \param mode How partially visible ChatLines are handled
     *  \return A list of visible ChatLines sorted by row
     *  \note If the order of ChatLines does not matter, use visibleChatLines() instead */
    QList<ChatLine *> visibleChatLinesSorted(Qt::ItemSelectionMode mode = Qt::ContainsItemBoundingRect) const;

    //! Return the last fully visible ChatLine in this view
    /** Using this method more efficient than calling visibleChatLinesSorted() and taking its last element.
     *  \return The last fully visible ChatLine in the view */
    ChatLine *lastVisibleChatLine(bool ignoreDayChange = false) const;

    //! Tell the view that this ChatLine has cached data
    /** ChatLines cache some layout data that should be cleared as soon as it's no
     *  longer visible. A ChatLine caching data registers itself with this method to
     *  tell the view about it. The view will call ChatLine::clearCache() when
     *  appropriate.
     *  \param line The ChatLine having cached data */
    void setHasCache(ChatLine *line, bool hasCache = true);

public slots:
    void clearCache();
    inline virtual void clear() {}

    void setMarkerLineVisible(bool visible = true);
    void setMarkerLine(MsgId msgId);
    void jumpToMarkerLine();

protected:
    virtual bool event(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void scrollContentsBy(int dx, int dy);

protected slots:
    virtual void verticalScrollbarChanged(int newPos);

private slots:
    void lastLineChanged(QGraphicsItem *chatLine, qreal offset);
    void adjustSceneRect();
    void checkChatLineCaches();
    void mouseMoveWhileSelecting(const QPointF &scenePos);
    void scrollTimerTimeout();
    void markerLineSet(MsgId msg);

private:
    ChatScene *_scene;
    int _lastScrollbarPos;
    QTimer _scrollTimer;
    int _scrollOffset;
    QSet<ChatLine *> _linesWithCache;
};

#endif // CHATVIEW_H
