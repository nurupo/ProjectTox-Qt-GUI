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

#include "chatviewsearchwidget.hpp"
#include "chatscene.hpp"
#include "chatitem.hpp"
#include "chatline.hpp"
#include <QLineEdit>
#include <QDebug>
#include "clickable.hpp"
#include "Settings/settings.hpp"
#include "smiley.hpp"
#include <QToolBar>

ChatViewSearchWidget::ChatViewSearchWidget(QWidget *parent) :
    QToolBar(parent),
    mScene(nullptr),
    mCaseSensitive(Qt::CaseInsensitive),
    mRegularMsgOnly(true)
{
    setHidden(true);
    setIconSize(QSize(16,16));

    mSearchDelayTimer.setSingleShot(true);

    mSearchLineEdit = new QLineEdit(this);
    mSearchLineEdit->setClearButtonEnabled(true);
    mSearchLineEdit->setPlaceholderText(tr("Search"));
    connect(mSearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(delaySearch()));
    connect(&mSearchDelayTimer, &QTimer::timeout, this, &ChatViewSearchWidget::search);

    QAction *csCheckbox = new QAction(QIcon(":/icons/text_uppercase.png"), tr("Case sensitive"), this);
    csCheckbox->setCheckable(true);
    connect(csCheckbox, &QAction::toggled, this, &ChatViewSearchWidget::setCaseSensitive);

    QAction *regularMsgCheckBox = new QAction(tr("Regular messages only"), this);
    regularMsgCheckBox->setCheckable(true);
    regularMsgCheckBox->setChecked(mRegularMsgOnly);
    connect(regularMsgCheckBox, &QAction::toggled, this, &ChatViewSearchWidget::setSearchOnlyRegularMsgs);

    QAction *prevButton = new QAction(QIcon(":/icons/resultset_previous.png"), tr("Previous search result"), this);
    prevButton->setShortcut(QKeySequence::FindPrevious);
    connect(prevButton, &QAction::triggered, this, &ChatViewSearchWidget::highlightPrev);

    QAction *nextButton = new QAction(QIcon(":/icons/resultset_next.png"), tr("Next search result"), this);
    nextButton->setShortcut(QKeySequence::FindNext);
    connect(nextButton, &QAction::triggered, this, &ChatViewSearchWidget::highlightNext);

    addWidget(mSearchLineEdit);
    addAction(prevButton);
    addAction(nextButton);
    addSeparator();
    addAction(csCheckbox);
    addAction(regularMsgCheckBox);
    addSeparator();
    addAction(QIcon(":/icons/cross.png"), tr("Close search"), this, SLOT(closeSearch()));
}

void ChatViewSearchWidget::setScene(ChatScene *scene)
{
    Q_ASSERT(scene);
    if (scene == mScene)
        return;

    if (mScene) {
        disconnect(mScene, 0, this, 0);
        clearHightlights();
    }

    mScene = scene;
    if (!scene)
        return;

    const Settings &s = Settings::getInstance();

    connect(mScene, SIGNAL(destroyed()), this, SLOT(sceneDestroyed()));
    connect(mScene, SIGNAL(rowsInserted()), this, SLOT(updateHighlights()));
    connect(&s, &Settings::smileyPackChanged, this, &ChatViewSearchWidget::updateExistingHighlights);
    // TODO MKO Backlog

    connect(mScene, SIGNAL(rowsAboutToBeRemoved(int,int)), this, SLOT(rowsRemoved(int,int)), Qt::DirectConnection); // Direct connection is important
}

void ChatViewSearchWidget::enableSearch(bool enable)
{
    mSearchEnabled = enable;
    updateHighlights();
}

void ChatViewSearchWidget::setSearchString(const QString &searchString)
{
    QString oldSearchString = mSearchString;
    mSearchString = searchString;
    if (mScene) {
        if (!searchString.startsWith(oldSearchString) || oldSearchString.isEmpty()) {
            // we can't reuse our all findings... clear the scene and do it all over
            updateHighlights();
        }
        else {
            // reuse all findings
            updateHighlights(true);
        }
    }
}

void ChatViewSearchWidget::setCaseSensitive(bool caseSensitive)
{
    if ((mCaseSensitive == Qt::CaseSensitive && caseSensitive) || (mCaseSensitive == Qt::CaseInsensitive && !caseSensitive))
        return;

    if(caseSensitive)
        mCaseSensitive = Qt::CaseSensitive;
    else
        mCaseSensitive = Qt::CaseInsensitive;

    // we can reuse the original search results if the new search parameters are a restriction of the original one
    updateHighlights(caseSensitive);
}

void ChatViewSearchWidget::setSearchOnlyRegularMsgs(bool searchOnlyRegularMsgs)
{
    if (mRegularMsgOnly == searchOnlyRegularMsgs)
        return;

    mRegularMsgOnly = searchOnlyRegularMsgs;

    // we can reuse the original search results if the new search parameters are a restriction of the original one
    updateHighlights(searchOnlyRegularMsgs);
}

void ChatViewSearchWidget::highlightNext()
{
    if (mHighlights.isEmpty())
        return;

    if (mCurrentHighlight < mHighlights.count()) {
        mHighlights[mCurrentHighlight]->setCurrent(false);
        mHighlights.at(mCurrentHighlight)->item()->chatLine()->update();
    }

    mCurrentHighlight++;
    if (mCurrentHighlight >= mHighlights.count())
        mCurrentHighlight = 0;

    mHighlights[mCurrentHighlight]->setCurrent(true);
    mHighlights.at(mCurrentHighlight)->item()->chatLine()->update();

    emit newCurrentHighlight(mHighlights.at(mCurrentHighlight)->item()->chatLine()->scenePos());
}

void ChatViewSearchWidget::highlightPrev()
{
    if (mHighlights.isEmpty())
        return;

    if (mCurrentHighlight < mHighlights.count()) {
        mHighlights[mCurrentHighlight]->setCurrent(false);
        mHighlights.at(mCurrentHighlight)->item()->chatLine()->update();
    }

    mCurrentHighlight--;
    if (mCurrentHighlight < 0)
        mCurrentHighlight = mHighlights.count() - 1;

    mHighlights[mCurrentHighlight]->setCurrent(true);
    mHighlights.at(mCurrentHighlight)->item()->chatLine()->update();

    emit newCurrentHighlight(mHighlights.at(mCurrentHighlight)->item()->chatLine()->scenePos());
}

// Remove all highlights in mHighlights-list of removed rows
void ChatViewSearchWidget::rowsRemoved(int start, int end)
{

    for (int i=0; i<mHighlights.count(); i++) {
        Highlight *h = mHighlights.at(i);
        if (!h)
            mHighlights.removeAt(mHighlights.indexOf(h));

        else if(h->item()->row()>= start && h->item()->row() <= end) {
            // Highlights in item will deleted by deleting item
            mHighlights.removeAt(mHighlights.indexOf(h));
            i--;
        }
    }

    // Find new current highlight
    updateCurrentHighlight(nullptr, 0);
}

void ChatViewSearchWidget::sceneDestroyed()
{
    mScene = nullptr;
    mHighlights.clear();
}

void ChatViewSearchWidget::updateHighlights(bool reuse)
{
    if (!mScene)
        return;

    if(!mSearchEnabled)
        return;

    if (reuse) {
        QSet<ChatItem *> chatItems;
        for (Highlight *highlight : mHighlights) {
            ChatItem *item = highlight->item();
            if (item)
                chatItems << item;
        }
        for (ChatItem *item : QList<ChatItem *>(chatItems.toList())) {
            updateHighlights(item);
        }
    }
    else {
        ChatItem *oldItem = nullptr;
        int oldStart = 0;
        if (!mHighlights.isEmpty() && mCurrentHighlight < mHighlights.count()) {
            oldItem = mHighlights.at(mCurrentHighlight)->item();
            oldStart = mHighlights.at(mCurrentHighlight)->start();
        }

        clearHightlights();
        Q_ASSERT(mHighlights.isEmpty());

        if (mSearchString.isEmpty())
            return;

        checkMessagesForHighlight();
        updateCurrentHighlight(oldItem, oldStart);
    }
}

// Find the nearest highlight to a given old one
void ChatViewSearchWidget::updateCurrentHighlight(ChatItem *oldItem, int oldStart)
{
    if (mHighlights.isEmpty())
        return;

    if (oldItem) {
        int start = 0;
        int end = mHighlights.count() - 1;

        Highlight *startPos;
        Highlight *endPos;

        while (1) {
            startPos = mHighlights.at(start);
            endPos = mHighlights.at(end);

            if (startPos->item()->row() == oldItem->row() && startPos->start() == oldStart) {
                mCurrentHighlight = start;
                break;
            }

            if (endPos->item()->row() == oldItem->row() && endPos->start() == oldStart) {
                mCurrentHighlight = end;
                break;
            }

            if (end - start == 1) {
                mCurrentHighlight = start;
                break;
            }

            if(end-start == 0) {
                qDebug() << "you shouldn't see this..."; // TODO MKO pivot search
                mCurrentHighlight = start;
                break;
            }

            int pivot = (end + start) / 2;
            Highlight *pivotPos = mHighlights.at(pivot);
            if (startPos->item()->row() == endPos->item()->row()) {
                if (oldStart <= pivotPos->start())
                    end = pivot;
                else
                    start = pivot;
            }
            else {
                if (oldItem->row() <= pivotPos->item()->row())
                    end = pivot;
                else
                    start = pivot;
            }
        }
    }
    else {
        mCurrentHighlight = mHighlights.count() - 1;
    }

    mHighlights.at(mCurrentHighlight)->setCurrent(true);
    mHighlights.at(mCurrentHighlight)->item()->chatLine()->update();
    emit newCurrentHighlight(mHighlights.at(mCurrentHighlight)->item()->chatLine()->scenePos());
}

void ChatViewSearchWidget::closeSearch()
{
    mSearchEnabled = false;
    clearHightlights();
    mSearchLineEdit->clear();
    hide();
}

void ChatViewSearchWidget::delaySearch()
{
    mSearchDelayTimer.start(300);
}

void ChatViewSearchWidget::search()
{
    setSearchString(mSearchLineEdit->text());
}

void ChatViewSearchWidget::checkMessagesForHighlight(int start, int end)
{
    QAbstractItemModel *model = mScene->model();
    Q_ASSERT(model);

    if (end == -1) {
        end = model->rowCount() - 1;
        if (end == -1)
            return;
    }

    QModelIndex index;
    for (int row = start; row <= end; row++) {
        if (mRegularMsgOnly) {
            index = model->index(row, 0);
            if (!checkType((Message::Type)index.data(MessageModel::TypeRole).toInt()))
                continue;
        }
        findHighlightInItem(mScene->chatLine(row)->contentsItem());
    }
}

// Find new highlights in item and highlight them
void ChatViewSearchWidget::findHighlightInItem(ChatItem *item)
{
    Offsets offset;
    offset.length = 0;
    offset.start  = 0;

    for (int idx : findWords(item->data(MessageModel::DisplayRole).toString())) {
        // Calculate smiley offsets
        if (item->type() == ChatScene::ContentsChatItemType) {
            ContentsChatItem *contentItem = static_cast<ContentsChatItem*>(item);
            offset = calculateSmileyOffsets(contentItem->smileyList(), idx);
        }

        if (mSearchString.count() + offset.length > 0)
            mHighlights << item->addHighlight(idx + offset.start, mSearchString.count() + offset.length);
    }
}

// This function updates only highlights of 1 item (delete and search again)
void ChatViewSearchWidget::updateHighlights(ChatItem *item)
{
    // Store the old current highlight
    ChatItem *oldItem = nullptr;
    int oldStart = 0;
    if (!mHighlights.isEmpty() && mCurrentHighlight < mHighlights.count()) {
        oldItem = mHighlights.at(mCurrentHighlight)->item();
        oldStart = mHighlights.at(mCurrentHighlight)->start();
    }

    // Delete all highlights of this item
    for (int i=0; i<item->highlights()->count(); i++) {
        Highlight *highlightItem = item->highlights()->at(i);
        if (!highlightItem)
            continue;

        highlightItem->item()->highlightRemove(highlightItem);
        mHighlights.removeAt(mHighlights.indexOf(highlightItem));
        i--;
    }

    // Find highlights of this item
    if (!mRegularMsgOnly || checkType((Message::Type)item->data(MessageModel::TypeRole).toInt())) {
        findHighlightInItem(item);
    }

    // set the current to the nearest highlight from the old one
    updateCurrentHighlight(oldItem, oldStart);

}

QList<int> ChatViewSearchWidget::findWords(const QString &string)
{
    QList<int> result;

    int searchIdx = 0;
    do {
        searchIdx = string.indexOf(mSearchString, searchIdx, mCaseSensitive);
        if(searchIdx!=-1) {
            result << searchIdx;
            searchIdx++;
        }
    } while (searchIdx != -1);

    return result;
}

void ChatViewSearchWidget::clearHightlights()
{
    for (Highlight *sc : mHighlights) {
        if(sc->item())
            sc->item()->highlightRemove(sc);
    }
    mHighlights.clear();
}

ChatViewSearchWidget::Offsets ChatViewSearchWidget::calculateSmileyOffsets(const SmileyList *smileys, int foundWordStart)
{
    Offsets r;
    r.length = 0;
    r.start  = 0;

    for (int i=0; i<smileys->count(); i++) {
        const Smiley s = smileys->at(i);
        qint8 graphicsLength = (s.type() == Smiley::Emoji) ? s.graphics().count() : 1;
        int searchStart      = foundWordStart;
        int searchEnd        = foundWordStart + mSearchString.count() - 1;
        int smileStart       = s.start();
        int smileEnd         = s.start() + s.textLength() - 1;

        // Julians algorithm
        if (smileStart < searchStart) {
            if (smileEnd < searchStart) {
                r.start += graphicsLength - s.textLength();
            }
            else if (smileEnd >= searchStart) {
                if (smileEnd < searchEnd) {
                    r.start += smileStart + graphicsLength - searchStart;
                    r.length += searchStart - smileEnd - 1;
                }
                else {
                    r.length -= mSearchString.count();
                }
            }
        }
        else if (smileStart >= searchStart && smileStart <= searchEnd) {
            if (smileEnd <= searchEnd) {
                r.length += graphicsLength - s.textLength();
            }
            else {
                r.length += smileStart - searchEnd - 1;
            }
        }
    }
    return r;
}
