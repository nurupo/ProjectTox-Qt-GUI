#include "messagemodelitem.hpp"
#include "messagemodel.hpp"

#include <QFontMetrics>
#include <QTextBoundaryFinder>
#include <QTextLayout>

// This Struct is taken from Harfbuzz. We use it only to calc it's size.
// we use a shared memory region so we do not have to malloc a buffer area for every line
typedef struct {
    /*HB_LineBreakType*/ unsigned lineBreakType  : 2;
    /*HB_Bool*/ unsigned whiteSpace              : 1;     /* A unicode whitespace character, except NBSP, ZWNBSP */
    /*HB_Bool*/ unsigned charStop                : 1;     /* Valid cursor position (for left/right arrow) */
    /*HB_Bool*/ unsigned wordBoundary            : 1;
    /*HB_Bool*/ unsigned sentenceBoundary        : 1;
    unsigned unused                  : 2;
} HB_CharAttributes_Dummy;

unsigned char *MessageModelItem::TextBoundaryFinderBuffer = (unsigned char *)malloc(512 * sizeof(HB_CharAttributes_Dummy));
int MessageModelItem::TextBoundaryFinderBufferSize = 512 * (sizeof(HB_CharAttributes_Dummy) / sizeof(unsigned char));

MessageModelItem::MessageModelItem(const Message &msg) :
    _styledMsg(msg)
{
    if (!msg.sender().contains('!'))
        _styledMsg.setFlags(msg.flags() |= Message::ServerMsg);
}

QVariant MessageModelItem::data(int column, int role) const
{
    if (column < 0 || column > 2)
        return QVariant();

    QVariant variant;
    MessageModel::ColumnType col = (MessageModel::ColumnType)column;
    switch (col) {
    case MessageModel::TimestampColumn:
        variant = timestampData(role);
        break;
    case MessageModel::SenderColumn:
        variant = senderData(role);
        break;
    case MessageModel::ContentsColumn:
        variant = contentsData(role);
        break;
    default:
        break;
    }
    if (variant.isValid())
        return variant;



    switch (role) {
    case MessageModel::MessageRole:
        return QVariant::fromValue<Message>(message());
    case MessageModel::MsgIdRole:
        return QVariant::fromValue<MsgId>(msgId());
    case MessageModel::TypeRole:
        return msgType();
    case MessageModel::FlagsRole:
        return (int)msgFlags();
    case MessageModel::TimestampRole:
        return timestamp();
    default:
        return QVariant();
    }
}

bool MessageModelItem::setData(int column, const QVariant &value, int role)
{
    Q_UNUSED(column);

    switch (role) {
    case MessageModel::FlagsRole:
        _styledMsg.setFlags((Message::Flags)value.toUInt());
        return true;
    default:
        return false;
    }
}

// Stuff for later
bool MessageModelItem::lessThan(const MessageModelItem *m1, const MessageModelItem *m2)
{
    return (*m1) < (*m2);
}

QVariant MessageModelItem::timestampData(int role) const
{
    switch (role) {
        case MessageModel::DisplayRole:
            return _styledMsg.decoratedTimestamp();
        case MessageModel::EditRole:
            return _styledMsg.timestamp();
        case MessageModel::BackgroundRole:
            return backgroundBrush(UiStyle::Timestamp);
        case MessageModel::SelectedBackgroundRole:
            return backgroundBrush(UiStyle::Timestamp, true);
        case MessageModel::FormatRole:
            return QVariant::fromValue<UiStyle::FormatList>(UiStyle::FormatList()
                                                            << qMakePair((quint16)0, (quint32) UiStyle::formatType(_styledMsg.type()) | UiStyle::Timestamp));
        }
        return QVariant();
}

QVariant MessageModelItem::senderData(int role) const
{
    switch (role) {
        case MessageModel::DisplayRole:
            return _styledMsg.decoratedSender();
        case MessageModel::EditRole:
            return _styledMsg.plainSender();
        case MessageModel::BackgroundRole:
            return backgroundBrush(UiStyle::Sender);
        case MessageModel::SelectedBackgroundRole:
            return backgroundBrush(UiStyle::Sender, true);
        case MessageModel::FormatRole:
            return QVariant::fromValue<UiStyle::FormatList>(UiStyle::FormatList()
                                                            << qMakePair((quint16)0, (quint32) UiStyle::formatType(_styledMsg.type()) | UiStyle::Sender));
        }
        return QVariant();
}

QVariant MessageModelItem::contentsData(int role) const
{
    switch (role) {
        case MessageModel::DisplayRole:
        case MessageModel::EditRole:
            return _styledMsg.plainContents();
        case MessageModel::BackgroundRole:
            return backgroundBrush(UiStyle::Contents);
        case MessageModel::SelectedBackgroundRole:
            return backgroundBrush(UiStyle::Contents, true);
        case MessageModel::FormatRole:
            return QVariant::fromValue<UiStyle::FormatList>(_styledMsg.contentsFormatList());
        case MessageModel::WrapListRole:
            if (_wrapList.isEmpty())
                computeWrapList();
            return QVariant::fromValue<MessageModel::WrapList>(_wrapList);
        }
        return QVariant();
}

QVariant MessageModelItem::backgroundBrush(UiStyle::FormatType subelement, bool selected) const
{
    QTextCharFormat fmt =  UiStyle::getInstance().format(UiStyle::formatType(_styledMsg.type()) | subelement, messageLabel() | (selected ? UiStyle::Selected : 0));
    if (fmt.hasProperty(QTextFormat::BackgroundBrush))
        return QVariant::fromValue<QBrush>(fmt.background());
    return QVariant();
}

// TODO MKO SY Was ist das label?
quint32 MessageModelItem::messageLabel() const
{
    quint32 label = _styledMsg.senderHash() << 16;
    if (_styledMsg.flags() & Message::Self)
        label |= UiStyle::OwnMsg;
    if (_styledMsg.flags() & Message::Highlight)
        label |= UiStyle::Highlight;
    return label;
}

void MessageModelItem::computeWrapList() const
{
    QString text = _styledMsg.plainContents();
    int length = text.length();
    if (!length)
        return;

    QList<MessageModel::Word> wplist; // use a temp list which we'll later copy into a QVector for efficiency
    QTextBoundaryFinder finder(QTextBoundaryFinder::Line, _styledMsg.plainContents().unicode(), length,
                               TextBoundaryFinderBuffer, TextBoundaryFinderBufferSize);

    int idx;
    int oldidx = 0;
    MessageModel::Word word;
    word.start = 0;
    qreal wordstartx = 0;

    QTextLayout layout(_styledMsg.plainContents());
    QTextOption option;
    option.setWrapMode(QTextOption::NoWrap);
    layout.setTextOption(option);

    layout.setAdditionalFormats(UiStyle::getInstance().toTextLayoutList(_styledMsg.contentsFormatList(), length, messageLabel()));
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setNumColumns(length);
    layout.endLayout();

    while ((idx = finder.toNextBoundary()) >= 0 && idx <= length) {
        // QTextBoundaryFinder has inconsistent behavior in Qt version up to and including 4.6.3 (at least).
        // It doesn't point to the position we should break, but to the character before that.
        // Unfortunately Qt decided to fix this by changing the behavior of QTBF, so now we have to add a version
        // check. At the time of this writing, I'm still trying to get this reverted upstream...
        //
        // cf. https://bugs.webkit.org/show_bug.cgi?id=31076 and Qt commit e6ac173
        static int needWorkaround = -1;
        if (needWorkaround < 0) {
            needWorkaround = 0;
            QStringList versions = QString(qVersion()).split('.');
            if (versions.count() == 3 && versions.at(0).toInt() == 4) {
                if (versions.at(1).toInt() <= 6 && versions.at(2).toInt() <= 3)
                    needWorkaround = 1;
            }
        }
        if (needWorkaround == 1) {
            if (idx < length)
                idx++;
        }

        if (idx == oldidx)
            continue;

        word.start = oldidx;
        int wordend = idx;
        for (; wordend > word.start; wordend--) {
            if (!text.at(wordend-1).isSpace())
                break;
        }

        qreal wordendx = line.cursorToX(wordend);
        qreal trailingendx = line.cursorToX(idx);
        word.endX = wordendx;
        word.width = wordendx - wordstartx;
        word.trailing = trailingendx - wordendx;
        wordstartx = trailingendx;
        wplist.append(word);

        oldidx = idx;
    }

    // A QVector needs less space than a QList
    _wrapList.resize(wplist.count());
    for (int i = 0; i < wplist.count(); i++) {
        _wrapList[i] = wplist.at(i);
    }
}


bool MessageModelItem::operator<(const MessageModelItem &other) const
{
    return msgId() < other.msgId();
}


bool MessageModelItem::operator==(const MessageModelItem &other) const
{
    return msgId() == other.msgId();
}


bool MessageModelItem::operator>(const MessageModelItem &other) const
{
    return msgId() > other.msgId();
}


QDebug operator<<(QDebug dbg, const MessageModelItem &msgItem)
{
    dbg.nospace() << qPrintable(QString("MessageModelItem(MsgId:")) << msgItem.msgId()
    << qPrintable(QString(",")) << msgItem.timestamp()
    << qPrintable(QString(", Type:")) << msgItem.msgType()
    << qPrintable(QString(", Flags:")) << msgItem.msgFlags() << qPrintable(QString(")"))
    << msgItem.data(1, Qt::DisplayRole).toString() << ":" << msgItem.data(2, Qt::DisplayRole).toString();
    return dbg;
}
