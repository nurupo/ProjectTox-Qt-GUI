/*
    Copyright (C) 2014 by Martin Kröll <technikschlumpf@web.de>

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

#ifndef CHATVIEWSEARCHWIDGET_HPP
#define CHATVIEWSEARCHWIDGET_HPP

#include <QToolBar>
#include <QTimer>
#include "message.hpp"

class ChatScene;
class QTextCursor;
class ChatItem;
class ChatLine;
class QLineEdit;
class Highlight;
class SmileyList;

class ChatViewSearchWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit ChatViewSearchWidget(QWidget *parent = 0);
    void setScene(ChatScene *scene);

public slots:
    void enableSearch(bool enable = true);
    void setSearchString(const QString &searchString);
    void setCaseSensitive(bool caseSensitive);
    void setSearchOnlyRegularMsgs(bool searchOnlyRegularMsgs);
    void highlightNext();
    void highlightPrev();
    void rowsRemoved(int start, int end);

signals:
    void newCurrentHighlight(QPointF position);

private slots:
    void sceneDestroyed();
    void updateHighlights(bool reuse = false);
    inline void updateExistingHighlights() { updateHighlights(true); }
    void closeSearch();
    void delaySearch();
    void search();

private:
    struct Offsets {
        int length;
        int start;
    };

    void checkMessagesForHighlight(int start = 0, int end = -1);
    void findHighlightInItem(ChatItem *item);
    void updateHighlights(ChatItem *item);
    void updateCurrentHighlight(ChatItem *oldItem, int oldStart);
    inline bool checkType(Message::Type type) const { return type & (Message::Plain | Message::Action); }
    QList<int> findWords(const QString &string);
    void clearHightlights();
    Offsets calculateSmileyOffsets(const SmileyList *smileys, int foundWordStart);

    bool mSearchEnabled;
    ChatScene *mScene;
    QString mSearchString;
    QLineEdit *mSearchLineEdit;
    Qt::CaseSensitivity mCaseSensitive;
    bool mRegularMsgOnly;
    QTimer mSearchDelayTimer;

    int mCurrentHighlight;
    QList<Highlight*> mHighlights; // A list of pointers to all highlights in all ContentItems
};

#endif // CHATVIEWSEARCHWIDGET_HPP
