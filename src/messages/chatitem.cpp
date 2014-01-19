#include "chatitem.hpp"
#include "chatline.hpp"
#include "chatview.hpp"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QMenu>
#include <QFontMetricsF>

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

void ChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());
    paintBackground(painter);

    layout()->draw(painter, pos(), QVector<QTextLayout::FormatRange>(), boundingRect());

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

void ChatItem::paintBackground(QPainter *painter)
{
    QBrush brush;
    if (_selectionMode == FullSelection)
        brush.setColor(Qt::red);
    else
        brush.setColor(Qt::yellow);

    painter->fillRect(boundingRect(), brush);
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

void SenderChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());
    paintBackground(painter);

    qreal layoutWidth = layout()->minimumWidth();
    qreal offset = 0;
    if (chatScene()->senderCutoffMode() == ChatScene::CutoffLeft)
        offset = qMin(width() - layoutWidth, (qreal)0);
    else
        offset = qMax(layoutWidth - width(), (qreal)0);

    if (layoutWidth > width()) {
        QLinearGradient gradient;
        if (offset < 0) {
            gradient.setStart(0, 0);
            gradient.setFinalStop(12, 0);
            gradient.setColorAt(0, Qt::transparent);
            gradient.setColorAt(1, Qt::white);
        }
        else {
            gradient.setStart(width()-10, 0);
            gradient.setFinalStop(width(), 0);
            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, Qt::transparent);
        }

        QPixmap pixmap(layout()->boundingRect().toRect().size());
        pixmap.fill(Qt::transparent);

        QPainter pixPainter(&pixmap);
        pixPainter.setCompositionMode(QPainter::CompositionMode_Source);
        layout()->draw(&pixPainter, QPointF(qMax(offset, (qreal)0), 0));
        pixPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        pixPainter.fillRect(0,0, pixmap.size().width(), pixmap.size().height(), gradient);
        pixPainter.end();

        painter->drawPixmap(pos(), pixmap);
    }
    else {
        layout()->draw(painter, pos(), QVector<QTextLayout::FormatRange>(), boundingRect());
    }
    painter->restore();
}

void SenderChatItem::initLayout(QTextLayout *layout) const
{
    initLayoutHelper(layout, QTextOption::ManualWrap, Qt::AlignRight);
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
    setGeometryByWidth(width);
}

ContentsChatItem::~ContentsChatItem()
{
    delete _data;
}

void ContentsChatItem::clearCache()
{
    delete _data;
    _data = 0;
    ChatItem::clearCache();
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
    QVector<QTextLayout::FormatRange> fmt;
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
    doLayout(layout);
}

void ContentsChatItem::doLayout(QTextLayout *layout) const
{
    MessageModel::WrapList wrapList = data(MessageModel::WrapListRole).value<MessageModel::WrapList>();
    if (!wrapList.count()) return;  // empty chatitem

    qreal h = 0;
    QFontMetricsF fm = QFontMetricsF(QFont()); // TODO MKO
    qreal spacing = qMax(fm.lineSpacing(), fm.height()); // cope with negative leading()
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
    // We use this for reloading layout info as well, so we can't bail out if the width doesn't change

    // compute height
    int lines = 1;
    WrapColumnFinder finder(this);
    while (finder.nextWrapColumn(w) > 0)
        lines++;

    QFontMetricsF fm = QFontMetricsF(QFont()); // TODO MKO
    qreal spacing = qMax(fm.lineSpacing(), fm.height()); // cope with negative leading()
    qreal h = lines * spacing;
    delete _data;
    _data = 0;

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

void TimestampChatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->save();
    painter->setClipRect(boundingRect());
    paintBackground(painter);

    qreal layoutWidth = layout()->minimumWidth();

    if (layoutWidth > width()) {
        QLinearGradient gradient;
            gradient.setStart(width()-10, 0);
            gradient.setFinalStop(width(), 0);
            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, Qt::transparent);

        QPixmap pixmap(layout()->boundingRect().toRect().size());
        pixmap.fill(Qt::transparent);

        QPainter pixPainter(&pixmap);
        pixPainter.setCompositionMode(QPainter::CompositionMode_Source);
        layout()->draw(&pixPainter, QPointF(0, 0));
        pixPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        pixPainter.fillRect(0,0, pixmap.size().width(), pixmap.size().height(), gradient);
        pixPainter.end();

        painter->drawPixmap(pos(), pixmap);
    }
    else {
        layout()->draw(painter, pos(), QVector<QTextLayout::FormatRange>(), boundingRect());
    }
    painter->restore();
}
