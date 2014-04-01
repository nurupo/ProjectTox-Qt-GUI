#include "chatviewsearchwidget.hpp"
#include "chatscene.hpp"
#include <QTextCursor>
#include "chatitem.hpp"
#include "chatline.hpp"
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "clickable.hpp"
#include "Settings/settings.hpp"
#include "smiley.hpp"

ChatViewSearchWidget::ChatViewSearchWidget(QWidget *parent) :
    QWidget(parent),
    _scene(0),
    _caseSensitive(Qt::CaseInsensitive),
    _searchOnlyRegularMsgs(true)
{
    setHidden(true);

    // Create widgets
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setPlaceholderText(tr("Search..."));
    connect(searchLineEdit, &QLineEdit::textChanged, this, &ChatViewSearchWidget::setSearchString);

    QToolButton *caseSensitiveCheckBox = new QToolButton(this);
    caseSensitiveCheckBox->setCheckable(true);
    caseSensitiveCheckBox->setAutoRaise(true);
    caseSensitiveCheckBox->setToolTip(tr("Case sensitive"));
    caseSensitiveCheckBox->setIcon(QIcon("://icons/text_uppercase.png"));
    connect(caseSensitiveCheckBox, &QCheckBox::toggled, this, &ChatViewSearchWidget::setCaseSensitive);

    QCheckBox *regularMsgCheckBox = new QCheckBox(tr("Regular messages only"), this);
    regularMsgCheckBox->setChecked(_searchOnlyRegularMsgs);
    connect(regularMsgCheckBox, &QCheckBox::toggled, this, &ChatViewSearchWidget::setSearchOnlyRegularMsgs);

    QToolButton *prevButton = new QToolButton(this);
    prevButton->setAutoRaise(true);
    prevButton->setIcon(QIcon(":/icons/resultset_previous.png"));
    prevButton->setToolTip(tr("Previous search result"));
    connect(prevButton, &QToolButton::clicked, this, &ChatViewSearchWidget::highlightPrev);

    QToolButton *nextButton = new QToolButton(this);
    nextButton->setAutoRaise(true);
    nextButton->setIcon(QIcon(":/icons/resultset_next.png"));
    nextButton->setToolTip(tr("Next search result"));
    nextButton->setShortcut(QKeySequence::FindNext);
    connect(nextButton, &QToolButton::clicked, this, &ChatViewSearchWidget::highlightNext);

    QToolButton *closeButton = new QToolButton(this);
    closeButton->setAutoRaise(true);
    closeButton->setIcon(QIcon(":/icons/cross.png"));
    connect(closeButton, &QToolButton::clicked, this, &ChatViewSearchWidget::closeSearch);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->setSpacing(2);
    searchLayout->setContentsMargins(0,0,0,0);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(caseSensitiveCheckBox);
    searchLayout->addWidget(prevButton);
    searchLayout->addWidget(nextButton);
    searchLayout->addStretch(1);
    searchLayout->addWidget(closeButton);

    QHBoxLayout *checkLayout = new QHBoxLayout;
    checkLayout->setSpacing(0);
    checkLayout->setContentsMargins(0,0,0,0);
    checkLayout->addWidget(regularMsgCheckBox);
    checkLayout->addStretch(1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addLayout(searchLayout);
    layout->addLayout(checkLayout);
    this->setLayout(layout);
}

void ChatViewSearchWidget::setScene(ChatScene *scene)
{
    Q_ASSERT(scene);
    if (scene == _scene)
        return;

    if (_scene) {
        disconnect(_scene, 0, this, 0);
        mHighlights.clear();
    }

    _scene = scene;
    if (!scene)
        return;

    const Settings &s = Settings::getInstance();

    connect(_scene, SIGNAL(destroyed()), this, SLOT(sceneDestroyed()));
    connect(_scene, SIGNAL(rowsInserted()), this, SLOT(updateHighlights()));
    connect(&s, &Settings::smileyPackChanged, this, &ChatViewSearchWidget::updateExistingHighlights);
    // TODO MKO Backlog

    updateHighlights();
}

void ChatViewSearchWidget::setSearchString(const QString &searchString)
{
    QString oldSearchString = _searchString;
    _searchString = searchString;
    if (_scene) {
        if (!searchString.startsWith(oldSearchString) || oldSearchString.isEmpty()) {
            // we can't reuse our all findings... cler the scene and do it all over
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
    if ((_caseSensitive == Qt::CaseSensitive && caseSensitive) || (_caseSensitive == Qt::CaseInsensitive && !caseSensitive))
        return;

    if(caseSensitive)
        _caseSensitive = Qt::CaseSensitive;
    else
        _caseSensitive = Qt::CaseInsensitive;

    // we can reuse the original search results if the new search parameters are a restriction of the original one
    updateHighlights(caseSensitive);
}

void ChatViewSearchWidget::setSearchOnlyRegularMsgs(bool searchOnlyRegularMsgs)
{
    if (_searchOnlyRegularMsgs == searchOnlyRegularMsgs)
        return;

    _searchOnlyRegularMsgs = searchOnlyRegularMsgs;

    // we can reuse the original search results if the new search parameters are a restriction of the original one
    updateHighlights(searchOnlyRegularMsgs);
}

void ChatViewSearchWidget::highlightNext()
{
    if (mHighlights.isEmpty())
        return;

    if (_currentHighlight < mHighlights.count()) {
        //SearchItemCursor scCur = mHighlights[_currentHighlight];
        //scCur.item->highlightFoundWord(scCur.start, scCur.end, false);
        mHighlights[_currentHighlight]->setCurrent(false);
        mHighlights.at(_currentHighlight)->item()->chatLine()->update();
        //scCur.item->setHighlight(scCur.start, scCur.end - scCur.start, false);
        //qDebug() << "de-highlight" << scCur.item->data(MessageModel::DisplayRole).toString().mid(scCur.start, scCur.end - scCur.start);
    }

    _currentHighlight++;
    if (_currentHighlight >= mHighlights.count())
        _currentHighlight = 0;

    mHighlights[_currentHighlight]->setCurrent(true);
    mHighlights.at(_currentHighlight)->item()->chatLine()->update();
    //SearchItemCursor scCur = _highlightCursors[_currentHighlight];
    //scCur.item->highlightFoundWord(scCur.start, scCur.end, true);
    //scCur.item->setHighlight(scCur.start, scCur.end - scCur.start, true);
    //qDebug() << "   Highlight" << scCur.item->data(MessageModel::DisplayRole).toString().mid(scCur.start, scCur.end - scCur.start);
    // emit newCurrentHighlight(_highlightItems.at(_currentHighlight));
}

void ChatViewSearchWidget::highlightPrev()
{
    if (mHighlights.isEmpty())
        return;

    if (_currentHighlight < mHighlights.count()) {
        mHighlights[_currentHighlight]->setCurrent(false);
        mHighlights.at(_currentHighlight)->item()->chatLine()->update();
        //SearchItemCursor scCur = _highlightCursors[_currentHighlight];
        //scCur.item->highlightFoundWord(scCur.start, scCur.end, false);
        //scCur.item->setHighlight(scCur.start, scCur.end - scCur.start, false);
        //qDebug() << "de-highlight" << scCur.item->data(MessageModel::DisplayRole).toString().mid(scCur.start, scCur.end - scCur.start);
    }

    _currentHighlight--;
    if (_currentHighlight < 0)
        _currentHighlight = mHighlights.count() - 1;

    mHighlights[_currentHighlight]->setCurrent(true);
    mHighlights.at(_currentHighlight)->item()->chatLine()->update();
    //SearchItemCursor scCur = _highlightCursors[_currentHighlight];
    //scCur.item->highlightFoundWord(scCur.start, scCur.end, true);
    //scCur.item->setHighlight(scCur.start, scCur.end - scCur.start, true);
    //qDebug() << "   Highlight" << scCur.item->data(MessageModel::DisplayRole).toString().mid(scCur.start, scCur.end - scCur.start);
    // emit newCurrentHighlight(_highlightItems.at(_currentHighlight));
}

void ChatViewSearchWidget::sceneDestroyed()
{
    _scene = nullptr;
    mHighlights.clear();
}

void ChatViewSearchWidget::updateHighlights(bool reuse)
{
    if (!_scene)
        return;

    reuse = false;
    if (reuse) {
        QSet<ChatItem *> chatItems;
        for (Hightlight *highlight : mHighlights) {
            ChatItem *item = highlight->item();
            if (item)
                chatItems << item;
        }
        for (ChatItem *item : QList<ChatItem *>(chatItems.toList())) {
            item->highlightsClear();
            updateHighlights(item);
        }
    }
    else {
        ChatItem *oldItem = nullptr;
        int oldStart = 0;
        if (!mHighlights.isEmpty() && _currentHighlight < mHighlights.count()) {
            oldItem = mHighlights.at(_currentHighlight)->item();
            oldStart = mHighlights.at(_currentHighlight)->start();
        }

        clearHightlights();
        Q_ASSERT(mHighlights.isEmpty());

        if (_searchString.isEmpty())
            return;

        checkMessagesForHighlight();

        if (mHighlights.isEmpty())
            return;

        if (oldItem) {
            int start = 0;
            int end = mHighlights.count() - 1;

            Hightlight *startPos;
            Hightlight *endPos;

            while (1) {
                startPos = mHighlights.at(start);
                endPos = mHighlights.at(end);

                if (startPos->item()->row() == oldItem->row() && startPos->start() == oldStart) {
                    _currentHighlight = start;
                    break;
                }

                if (endPos->item()->row() == oldItem->row() && endPos->start() == oldStart) {
                    _currentHighlight = end;
                    break;
                }

                if (end - start == 1) {
                    _currentHighlight = start;
                    break;
                }

                if(end-start == 0) {
                    qDebug() << "hmm suchen warum pivot"; // TODO MKO
                    break;
                }

                int pivot = (end + start) / 2;
                Hightlight *pivotPos = mHighlights.at(pivot);
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
            _currentHighlight = mHighlights.count() - 1;
        }

        mHighlights.at(_currentHighlight)->setCurrent(true);
        mHighlights.at(_currentHighlight)->item()->chatLine()->update();
        //SearchItemCursor scCur = _highlightCursors[_currentHighlight];
        //scCur.item->highlightFoundWord(scCur.start, scCur.end, true);
        //scCur.item->setHighlight(scCur.start, scCur.end - scCur.start, true);
        //qDebug() << "newCurrentHighlight" << scCur.item->data(MessageModel::DisplayRole).toString().mid(scCur.start, scCur.end - scCur.start);
        //_highlightCursors[_currentHighlight]->setHighlighted(true);
        //emit newCurrentHighlight(_highlightItems[_currentHighlight]);
    }
}

void ChatViewSearchWidget::closeSearch()
{
    clearHightlights();
    searchLineEdit->clear();
    hide();
}

void ChatViewSearchWidget::checkMessagesForHighlight(int start, int end)
{
    QAbstractItemModel *model = _scene->model();
    Q_ASSERT(model);

    if (end == -1) {
        end = model->rowCount() - 1;
        if (end == -1)
            return;
    }

    QModelIndex index;
    for (int row = start; row <= end; row++) {
        if (_searchOnlyRegularMsgs) {
            index = model->index(row, 0);
            if (!checkType((Message::Type)index.data(MessageModel::TypeRole).toInt()))
                continue;
        }
        highlightLine(_scene->chatLine(row)->contentsItem());
    }
}

// find highlights in item and highlight it
void ChatViewSearchWidget::highlightLine(ChatItem *line) // TODO MKO ubenennen
{
    ChatItem *item = line;

    // adjust highlight length for smileys
    QString text = item->data(MessageModel::DisplayRole).toString();
    SmileyList smileys = SmileyList::fromText(text);

    foreach (Smiley s, smileys) {
        qDebug() << s.start() << s.text();
    }


    for (int idx : findWords(item->data(MessageModel::DisplayRole).toString())) {

       Offsets offset = calculateSmileyOffsets(&smileys, idx);

       if(_searchString.count() + offset.length > 0)
            mHighlights << item->setHighlight(idx + offset.start, _searchString.count() + offset.length);

       // give highlight to item without care of smiley offsets
        //mHighlights << item->setHighlight(idx, _searchString.count());
    }
}

// This function updates only highlights of an item (delete and search again)
void ChatViewSearchWidget::updateHighlights(ChatItem *item)
{
    // adjust highlight length for smileys
    //QString text = item->data(MessageModel::DisplayRole).toString();
    //SmileyList smileys = SmileyList::smilify(text);

    //QList<int> wordFounds = findWords(item->data(MessageModel::DisplayRole).toString());

    //bool deleteAll = false;
    QAbstractItemModel *model = _scene->model();
    Q_ASSERT(model);


    // Delete all highlights of this item
    for (int i=0; i<item->highlights()->count(); i++) {
        Hightlight *highlightItem = item->highlights()->at(i);
        if (!highlightItem)
            continue;

        int pos = mHighlights.indexOf(highlightItem);
        if (pos == _currentHighlight) {
            highlightPrev();
        }
        else if (pos < _currentHighlight) {
            _currentHighlight--;
        }

        highlightItem->item()->highlightRemove(highlightItem);
        mHighlights.removeAt(pos);
        i--;
    }

    // Find highlights of this line
    highlightLine(item);



    /*if (_searchOnlyRegularMsgs) {
        QModelIndex index = model->index(item->row(), 0);
        if (!checkType((Message::Type)index.data(MessageModel::TypeRole).toInt()))
            deleteAll = true;
    }*/


    // TODO MKO hier dran!!!!!!!! die smileypositionen mit offsetfunktion neu brerechnen

    // if it is a content item
    /*ContentsChatItem *contentItem = nullptr;
    if (highlightItem->item()->type() == ChatScene::ContentsChatItemType) {
        contentItem = static_cast<ContentsChatItem*>(highlightItem->item());
    }*/


    /*for (int i=0; i<item->highlights()->count(); i++) {
        Hightlight *highlightItem = item->highlights()->at(i);
        if (!highlightItem)
            continue;


        // 1. alle highlights löschen
        // 2. neue erstellen


        // HIER DRAN !!!!!!!!! Problem, der highlight hat ja schon einen offset, er soll ja nur verlängert werden
        Offsets offset = calculateSmileyOffsets(&smileys, highlightItem->start());

        if (!deleteAll && wordFounds.contains(highlightItem->start() + offset.start)) {

            highlightItem->setLength(_searchString.count() + offset.length);
            item->chatLine()->update();
        }
        else {
            int pos = mHighlights.indexOf(highlightItem);
            if (pos == _currentHighlight) {
                highlightPrev();
            }
            else if (pos < _currentHighlight) {
                _currentHighlight--;
            }

            highlightItem->item()->highlightRemove(highlightItem);
            mHighlights.removeAt(pos);
            i--;
        }
    }*/
}

QList<int> ChatViewSearchWidget::findWords(const QString &string)
{
    QList<int> result;

    int searchIdx = 0;
    do {
        searchIdx = string.indexOf(_searchString, searchIdx, _caseSensitive);
        if(searchIdx!=-1) {
            result << searchIdx;
            searchIdx++;
        }
    } while (searchIdx != -1);

    return result;
}

void ChatViewSearchWidget::clearHightlights()
{
    for (Hightlight *sc : mHighlights) {
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
        int searchEnd        = foundWordStart + _searchString.count() - 1;
        int smileStart       = s.start();
        int smileEnd         = s.start() + s.textLength() - 1;
        qDebug() << "searchStart:" << searchStart << "| searchEnd:" << searchEnd << "| smileStart:" << smileStart << "| smileEnd:" << smileEnd << "| _searchString.count()" << _searchString.count();

        // Julians algorithm
        if (smileStart < searchStart) {
            if (smileEnd < searchStart) {
                r.start += graphicsLength - s.textLength();
                qDebug() << "a";
            }
            else if (smileEnd >= searchStart) {
                if (smileEnd < searchEnd) {
                    r.start += smileStart + graphicsLength - searchStart;
                    r.length += searchStart - smileEnd - 1;
                    qDebug() << "b";
                }
                else {
                    r.length -= _searchString.count();
                    // TODO MKO offset auf ende smiley setzen
                    qDebug() << "c";
                }
            }
        }
        else if (smileStart >= searchStart && smileStart <= searchEnd) {
            if (smileEnd <= searchEnd) {
                r.length += graphicsLength - s.textLength();
                qDebug() << "d";
            }
            else {
                r.length += smileStart - searchEnd - 1;
                qDebug() << "e";
            }
        }

        qDebug() << "temp Offsets:" << r.start << r.length;

        /*
        // startet vor oder mit smiley und endet innerhalb smiley
        if (searchStart <= smileStart && searchEnd >= smileStart && searchEnd < smileEnd) {
            r.length -= searchEnd - smileStart + 1; // MKO TODO slength
        }
        // startet in und endet mit oder in smiley
        else if(searchStart > smileStart && searchEnd <= smileEnd) {
            r.length -= _searchString.count();
            r.start += smileStart + graphicsLength -searchStart;
        }
        // startet in smiley und endet nach smiley
        else if(searchStart > smileStart && searchStart <= smileEnd && searchEnd > smileEnd) {
            r.length -= smileEnd - searchStart + 1;
            r.start += smileStart + graphicsLength -searchStart;
        }
        // startet mit oder vor und endet mit oder nach smiley
        else if(searchStart <= smileStart && searchEnd >= smileEnd) {
            r.length -= s.textLength() - graphicsLength;
        }
        // startet nach smiley
        else if(searchStart > smileEnd) {
            r.start -= s.textLength() - graphicsLength;
        }
        // startet vor und endet vor smiley
        else if(searchStart < smileStart) {
            ;
        }
        else {
            qWarning() << "MKO SOLLTE NICHT PASSIEREN!";
        }
        */


        /*if ((idx >= s.start() && idx <= s.start() + s.textLength())
                || (idx < s.start() && idx + _searchString.count() > s.start() && idx + _searchString.count() < s.start() + s.textLength())) {
            offset += idx + _searchString.count() - s.start();
            break;
        } else if(idx < s.start() && idx + _searchString.count() > s.start()) {
            offset += s.textLength() - slength;
            break;
        }*/
    }
    qDebug() << QString("============================\n")
             << QString("start   : %1").arg(foundWordStart)         << QString("length  : %1\n").arg(_searchString.count())
             << QString("+ offset: %1").arg(r.start)                << QString("+ offset: %1\n").arg(r.length)
             << QString("-----------")                              << QString("-----------\n")
             << QString("        : %1").arg(foundWordStart+r.start) << QString("        : %1").arg(_searchString.count() + r.length);

    return r;
}

// ============================================================================


bool ChatViewSearchWidget::SearchItemCursor::operator==(const ChatViewSearchWidget::SearchItemCursor &other) const
{
    return (start == other.start && end == other.end && item->row() == other.item->row()) ? true : false;
}

uint qHash(const ChatViewSearchWidget::SearchItemCursor &key, uint seed)
{
    return qHash(key.item->row(), seed) ^ key.start ^ key.end; // TODO MKO ?????
}
