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

ChatItem::ChatItem(const QRectF &boundingRect, ChatLine *parent) :
    _parent(parent),
    _boundingRect(boundingRect),
    _selectionMode(NoSelection),
    _selectionStart(-1),
    _cachedLayout(0)
{
}

ChatItem::~ChatItem()
{
    delete _cachedLayout;
}

const QAbstractItemModel *ChatItem::model() const
{
    return chatLine()->model();
}

ChatLine *ChatItem::chatLine() const
{
    return _parent;
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

void ChatItem::paintBackground(QPainter *painter)
{
    QVariant bgBrush;
    if (_selectionMode == FullSelection)
        bgBrush = data(MessageModel::SelectedBackgroundRole);
    else
        bgBrush = data(MessageModel::BackgroundRole);
    if (bgBrush.isValid())
        painter->fillRect(boundingRect(), bgBrush.value<QBrush>());
}

// NOTE: This is not the most time-efficient implementation, but it saves space by not caching unnecessary data
//       This is a deliberate trade-off. (-> selectFmt creation, data() call)
void ChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());
    paintBackground(painter);

    layout()->draw(painter, pos(), additionalFormats(), boundingRect());
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
    if (_selectionMode == FullSelection)
        return data(MessageModel::DisplayRole).toString();
    if (_selectionMode == PartialSelection)
        return data(MessageModel::DisplayRole).toString().mid(qMin(_selectionStart, _selectionEnd), qAbs(_selectionStart - _selectionEnd));
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

QList<QRectF> ChatItem::findWords(const QString &searchWord, Qt::CaseSensitivity caseSensitive)
{
    QList<QRectF> resultList;
    const QAbstractItemModel *model_ = model();
    if (!model_)
        return resultList;

    QString plainText = model_->data(model_->index(row(), column()), MessageModel::DisplayRole).toString();
    QList<int> indexList;
    int searchIdx = plainText.indexOf(searchWord, 0, caseSensitive);
    while (searchIdx != -1) {
        indexList << searchIdx;
        searchIdx = plainText.indexOf(searchWord, searchIdx + 1, caseSensitive);
    }

    foreach(int idx, indexList) {
        QTextLine line = layout()->lineForTextPosition(idx);
        qreal x = line.cursorToX(idx);
        qreal width = line.cursorToX(idx + searchWord.count()) - x;
        qreal height = line.height();
        qreal y = height * line.lineNumber();
        resultList << QRectF(x, y, width, height);
    }

    return resultList;
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

void ChatItem::initLayoutHelper(QTextLayout *layout, QTextOption::WrapMode wrapMode, Qt::Alignment alignment) const
{
    Q_ASSERT(layout);

    layout->setText(data(MessageModel::DisplayRole).toString());


    QTextOption option;
    option.setWrapMode(wrapMode);
    option.setAlignment(alignment);
    layout->setTextOption(option);

    QList<QTextLayout::FormatRange> formatRanges = UiStyle::getInstance().toTextLayoutList(formatList(), layout->text().length(), data(MessageModel::MsgLabelRole).toUInt());
    layout->setAdditionalFormats(formatRanges);
}

void ChatItem::clearCache()
{
    delete _cachedLayout;
    _cachedLayout = 0;
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

QTextLayout *ChatItem::layout() const
{
    if (_cachedLayout)
        return _cachedLayout;

    _cachedLayout = new QTextLayout;
    initLayout(_cachedLayout);
    chatView()->setHasCache(chatLine());
    return _cachedLayout;
}

void ChatItem::initLayout(QTextLayout *layout) const
{
    initLayoutHelper(layout, QTextOption::NoWrap);
    doLayout(layout);
}

void ChatItem::doLayout(QTextLayout *layout) const
{
    layout->beginLayout();
    QTextLine line = layout->createLine();
    if (line.isValid()) {
        line.setLineWidth(width());
        line.setPosition(QPointF(0, 0));
    }
    layout->endLayout();
}

UiStyle::FormatList ChatItem::formatList() const
{
    return data(MessageModel::FormatRole).value<UiStyle::FormatList>();
}

QVector<QTextLayout::FormatRange> ChatItem::selectionFormats() const
{
    if (!hasSelection())
            return QVector<QTextLayout::FormatRange>();

        int start, end;
        if (_selectionMode == FullSelection) {
            start = 0;
            end = data(MessageModel::DisplayRole).toString().length();
        }
        else {
            start = qMin(_selectionStart, _selectionEnd);
            end = qMax(_selectionStart, _selectionEnd);
        }

        UiStyle::FormatList fmtList = formatList();

        while (fmtList.count() > 1 && fmtList.at(1).first <= start)
            fmtList.removeFirst();

        fmtList.first().first = start;

        while (fmtList.count() > 1 && fmtList.last().first >= end)
            fmtList.removeLast();

        return UiStyle::getInstance().toTextLayoutList(fmtList, end, UiStyle::Selected|data(MessageModel::MsgLabelRole).toUInt()).toVector();
}

QAbstractTextDocumentLayout::Selection ChatItem::selectionLayout() const
{
    QAbstractTextDocumentLayout::Selection selection;

    if (!hasSelection())
        return selection;

    int start;
    int end;

    if (_selectionMode == FullSelection) {
        start = 0;
        end = data(MessageModel::DisplayRole).toString().length();
    }
    else {
        start = _selectionStart;
        end   = _selectionEnd;
    }

    QTextCursor c(mDoc);
    c.setPosition(start);
    c.setPosition(end, QTextCursor::KeepAnchor);
    selection.cursor = c;

    return selection;
}


QVector<QTextLayout::FormatRange> ChatItem::additionalFormats() const
{
    return selectionFormats();
}

void ChatItem::overlayFormat(UiStyle::FormatList &fmtList, int start, int end, quint32 overlayFmt) const
{
    for (int i = 0; i < fmtList.count(); i++) {
        int fmtStart = fmtList.at(i).first;
        int fmtEnd = (i < fmtList.count()-1 ? fmtList.at(i+1).first : data(MessageModel::DisplayRole).toString().length());

        if (fmtEnd <= start)
            continue;
        if (fmtStart >= end)
            break;

        // split the format if necessary
        if (fmtStart < start) {
            fmtList.insert(i, fmtList.at(i));
            fmtList[++i].first = start;
        }
        if (end < fmtEnd) {
            fmtList.insert(i, fmtList.at(i));
            fmtList[i+1].first = end;
        }

        fmtList[i].second |= overlayFmt;
    }
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
        return data(MessageModel::DisplayRole).toString().length();
    if (pos.y() < 0)
        return 0;

    for (int l = layout()->lineCount() - 1; l >= 0; l--) {
        QTextLine line = layout()->lineAt(l);
        if (pos.y() >= line.y()) {
            return line.xToCursor(pos.x(), QTextLine::CursorOnCharacter);
        }
    }
    return 0;
}

// ************************************************************
// SenderChatItem
// ************************************************************

void SenderChatItem::initLayout(QTextLayout *layout) const
{
    initLayoutHelper(layout, QTextOption::NoWrap);

    // Hide Sender if is same as before
    QModelIndex lastIndex = model()->index(row()-1, column());
    if (lastIndex.isValid()
            && data(MessageModel::TypeRole).toInt() == Message::Plain
            && model()->data(lastIndex, MessageModel::TypeRole).toInt() == Message::Plain
            && (data(MessageModel::FlagsRole).toInt() & Message::Self) == (model()->data(lastIndex, MessageModel::FlagsRole).toInt() & Message::Self)) {
        layout->setText("");
    }
    doLayout(layout);
}

// ************************************************************
// ContentsChatItem
// ************************************************************

ContentsChatItem::ActionProxy ContentsChatItem::mActionProxy;

ContentsChatItem::ContentsChatItem(const QPointF &pos, const qreal &width, ChatLine *parent) :
    ChatItem(QRectF(pos, QSizeF(width, 0)), parent),
    _data(0)
{
    setPos(pos);
    mDoc = new QTextDocument;
    mDoc->setPlainText(data(MessageModel::DisplayRole).toString());
    setGeometryByWidth(width);
}

ContentsChatItem::~ContentsChatItem()
{
    delete _data;
    delete mDoc;
}

QFontMetricsF *ContentsChatItem::fontMetrics() const
{
    return UiStyle::getInstance().fontMetrics(data(MessageModel::FormatRole).value<UiStyle::FormatList>().at(0).second, 0);
}

void ContentsChatItem::clearCache()
{
    delete _data;
    _data = 0;
    ChatItem::clearCache();
}

void ContentsChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());
    //paintBackground(painter);

    //layout()->draw(painter, pos(), additionalFormats(), boundingRect());
    /*QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setTextWidth(boundingRect().width());
    QObject *smileyInterface = new SmileyTextObject;
    doc.documentLayout()->registerHandler(QTextFormat::UserObject + 1, smileyInterface);

    QTextCursor cursor(&doc);
    cursor.insertText(layout()->text());
*/
    /*QTextCharFormat smileyFormat;
    smileyFormat.setObjectType(QTextFormat::UserObject + 1);
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), smileyFormat);
    */



    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette = QApplication::palette("QWidgetTextControl"); // TODO MKO correct class name?


    // Selection
    QAbstractTextDocumentLayout::Selection selection = selectionLayout();
    QPalette::ColorGroup cg = chatView()->hasFocus() ? QPalette::Active : QPalette::Inactive;
    selection.format.setBackground(ctx.palette.brush(cg, QPalette::Highlight));
    selection.format.setForeground(ctx.palette.brush(cg, QPalette::HighlightedText));
    ctx.selections.append(selection);


    painter->translate(pos());
    mDoc->documentLayout()->draw(painter, ctx);

    painter->restore();
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
            chatLine()->setCursor(Qt::PointingHandCursor);
            privateData()->currentClickable = click;
            chatLine()->update();
            return;
        }
    }
    if (!onClickable) endHoverMode();
    event->accept();
}

void ContentsChatItem::handleClick(const QPointF &pos, ChatScene::ClickMode clickMode)
{
    if (clickMode == ChatScene::SingleClick) {
        qint16 idx = posToCursor(pos);
        Clickable foo = privateData()->clickables.atCursorPos(idx);
        if (foo.isValid()) {
            QString text = data(MessageModel::DisplayRole).toString();
            foo.activate(text);
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
            QString str = data(MessageModel::DisplayRole).toString();
            qint16 cursor = posToCursor(pos);
            qint16 start = str.lastIndexOf(QRegExp("\\W"), cursor) + 1;
            qint16 end = qMin(str.indexOf(QRegExp("\\W"), cursor), str.length());
            if (end < 0) end = str.length();
            setSelectionStart(start);
            setSelectionEnd(end);
        }
        chatLine()->update();
    }
    else if (clickMode == ChatScene::TripleClick) {
        setSelection(PartialSelection, 0, data(MessageModel::DisplayRole).toString().length());
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
        QString url = data(MessageModel::DisplayRole).toString().mid(click.start(), click.length());
        if (!url.contains("://"))
            url = "http://" + url;
        chatScene()->stringToClipboard(url);
    }
}

QVector<QTextLayout::FormatRange> ContentsChatItem::additionalFormats() const
{
    QVector<QTextLayout::FormatRange> fmt = ChatItem::additionalFormats();

    // mark a clickable if hovered upon
    if (privateData()->currentClickable.isValid()) {
        Clickable click = privateData()->currentClickable;
        QTextLayout::FormatRange f;
        f.start = click.start();
        f.length = click.length();
        f.format.setFontUnderline(true);
        fmt.append(f);
    }
    return fmt;
}

void ContentsChatItem::initLayout(QTextLayout *layout) const
{
    initLayoutHelper(layout, QTextOption::WrapAtWordBoundaryOrAnywhere);
    //doLayout(layout);
}
/*
void ContentsChatItem::doLayout(QTextLayout *layout) const
{
    MessageModel::WrapList wrapList = data(MessageModel::WrapListRole).value<MessageModel::WrapList>();
    if (!wrapList.count()) return;  // empty chatitem

    qreal h = 0;
    qreal spacing = qMax(fontMetrics()->lineSpacing(), fontMetrics()->height()); // cope with negative leading()
    WrapColumnFinder finder(this);
    layout->beginLayout();
    forever {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        int col = finder.nextWrapColumn(width());
        if (col < 0)
            col = layout->text().length();
        int num = col - line.textStart();

        line.setNumColumns(num);

        // Sometimes, setNumColumns will create a line that's too long (cf. Qt bug 238249)
        // We verify this and try setting the width again, making it shorter each time until the lengths match.
        // Dead fugly, but seems to work…
        for (int i = line.textLength()-1; i >= 0 && line.textLength() > num; i--) {
            line.setNumColumns(i);
        }
        if (num != line.textLength()) {
            qWarning() << "WARNING: Layout engine couldn't workaround Qt bug 238249, please report!";
            // qDebug() << num << line.textLength() << t.mid(line.textStart(), line.textLength()) << t.mid(line.textStart() + line.textLength());
        }

        line.setPosition(QPointF(0, h));
        h += spacing;
    }
    layout->endLayout();
}
*/
UiStyle::FormatList ContentsChatItem::formatList() const
{
    UiStyle::FormatList fmtList = ChatItem::formatList();
    for (int i = 0; i < privateData()->clickables.count(); i++) {
        Clickable click = privateData()->clickables.at(i);
        if (click.type() == Clickable::Url) {
            overlayFormat(fmtList, click.start(), click.start() + click.length(), UiStyle::Url);
        }
    }
    return fmtList;
}

qint16 ContentsChatItem::posToCursor(const QPointF &posInLine) const
{
    QPointF pos = mapFromLine(posInLine);
    if (pos.y() > height())
        return data(MessageModel::DisplayRole).toString().length();
    if (pos.y() < 0)
        return 0;

    int found = mDoc->documentLayout()->hitTest(pos, Qt::FuzzyHit);
    //qDebug() << found << data(MessageModel::DisplayRole).toString().length();

    if(found < 0)
        return 0;
    return found;
}

ContentsChatItemPrivate *ContentsChatItem::privateData() const
{
    if (!_data) {
        ContentsChatItem *that = const_cast<ContentsChatItem *>(this);
        that->_data = new ContentsChatItemPrivate(ClickableList::fromString(data(MessageModel::DisplayRole).toString()), that);
    }
    return _data;
}

Clickable ContentsChatItem::clickableAt(const QPointF &pos) const
{
    return privateData()->clickables.atCursorPos(posToCursor(pos));
}

void ContentsChatItem::endHoverMode()
{
    if (privateData()) {
        if (privateData()->currentClickable.isValid()) {
            chatLine()->unsetCursor();
            privateData()->currentClickable = Clickable();
        }
        chatLine()->update();
    }
}

qreal ContentsChatItem::setGeometryByWidth(qreal w)
{
    mDoc->setTextWidth(w);
    qreal h = mDoc->size().height();

    if (w != width() || h != height())
        setGeometry(w, h);

    return h;

}

/*************************************************************************************************/

ContentsChatItem::WrapColumnFinder::WrapColumnFinder(const ChatItem *_item)
    : item(_item),
    wrapList(item->data(MessageModel::WrapListRole).value<MessageModel::WrapList>()),
    wordidx(0),
    lineCount(0),
    choppedTrailing(0)
{
}

qint16 ContentsChatItem::WrapColumnFinder::nextWrapColumn(qreal width)
{
    if (wordidx >= wrapList.count())
        return -1;

    lineCount++;
    qreal targetWidth = lineCount * width + choppedTrailing;

    qint16 start = wordidx;
    qint16 end = wrapList.count() - 1;

    // check if the whole line fits
    if (wrapList.at(end).endX <= targetWidth) //  || start == end)
        return -1;

    // check if we have a very long word that needs inter word wrap
    if (wrapList.at(start).endX > targetWidth) {
        if (!line.isValid()) {
            item->initLayoutHelper(&layout, QTextOption::NoWrap);
            layout.beginLayout();
            line = layout.createLine();
            layout.endLayout();
        }
        return line.xToCursor(targetWidth, QTextLine::CursorOnCharacter);
    }

    while (true) {
        if (start + 1 == end) {
            wordidx = end;
            const MessageModel::Word &lastWord = wrapList.at(start); // the last word we were able to squeeze in

            // both cases should be cought preliminary
            Q_ASSERT(lastWord.endX <= targetWidth); // ensure that "start" really fits in
            Q_ASSERT(end < wrapList.count()); // ensure that start isn't the last word

            choppedTrailing += lastWord.trailing - (targetWidth - lastWord.endX);
            return wrapList.at(wordidx).start;
        }

        qint16 pivot = (end + start) / 2;
        if (wrapList.at(pivot).endX > targetWidth) {
            end = pivot;
        }
        else {
            start = pivot;
        }
    }
    Q_ASSERT(false);
    return -1;
}

// ************************************************************
// TimestampChatItem
// ************************************************************


void TimestampChatItem::initLayout(QTextLayout *layout) const
{
        initLayoutHelper(layout, QTextOption::ManualWrap, Qt::AlignRight);
        doLayout(layout);
}
