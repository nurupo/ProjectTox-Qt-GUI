#include "style.hpp"

#include <QTextCharFormat>
#include <QFontMetricsF>
#include <QApplication>
#include <QPalette>

QHash<QString, UiStyle::FormatType> UiStyle::_formatCodes;
QString UiStyle::_timestampFormatString;

UiStyle::UiStyle(QObject *parent) :
    QObject(parent)
{
    // register FormatList if that hasn't happened yet
    // FIXME I don't think this actually avoids double registration... then again... does it hurt?
    if (QVariant::nameToType("UiStyle::FormatList") == QVariant::Invalid) {
        qRegisterMetaType<FormatList>("UiStyle::FormatList");
        qRegisterMetaTypeStreamOperators<FormatList>("UiStyle::FormatList");
        Q_ASSERT(QVariant::nameToType("UiStyle::FormatList") != QVariant::Invalid);
    }

    _uiStylePalette = QVector<QBrush>(NumRoles, QBrush());

    // Now initialize the mapping between FormatCodes and FormatTypes...
    // TODO MKO SY brauch ich das?
    _formatCodes["%O"] = Base;
    _formatCodes["%B"] = Bold;
    _formatCodes["%S"] = Italic;
    _formatCodes["%U"] = Underline;
    _formatCodes["%R"] = Reverse;

    _formatCodes["%DN"] = Nick;
    _formatCodes["%DH"] = Hostmask;
    _formatCodes["%DC"] = ChannelName;
    _formatCodes["%DM"] = ModeFlags;
    _formatCodes["%DU"] = Url;

    // TODO MKO SY einstellbar Settings
    setTimestampFormatString("[hh:mm:ss]");

    // TODO MKO SY loadStylesheet gelöscht, eventuell Standardwerte angeben
    createFormat(Base,         Default,   QApplication::palette().text());
    createFormat(Base,         Highlight, Qt::black, Qt::green);
    createFormat(Base,         Selected,  QApplication::palette().highlightedText(), QApplication::palette().highlight());
    createFormat(Sender,       OwnMsg,    QApplication::palette().mid()); // TODO MKO SY klappt nicht...
    createFormat(Timestamp,    Default,   QApplication::palette().mid());
    createFormat(Url,          Default,   QApplication::palette().link());
    createFormat(ActionMsg,    Default,   QColor("#4E9A06"));
    createFormat(NickMsg,      Default,   QColor("#204A87"));
    createFormat(DayChangeMsg, Default,   QColor("#AD7FA8"));
}

/*! Create a new Format for the Style. */
void UiStyle::createFormat(UiStyle::FormatType type, UiStyle::MessageLabel label, QBrush foreground, QBrush background)
{
    quint64 fmtType = 0;
    fmtType |= type;
    fmtType |= ( (quint64) label << 32);

    QTextCharFormat format;
    format.setForeground(foreground);
    format.setBackground(background);

    _formats[fmtType].merge(format);
}

UiStyle &UiStyle::getInstance()
{
    static UiStyle style;
    return style;
}

UiStyle::~UiStyle()
{
    qDeleteAll(_metricsCache);
}

void UiStyle::setTimestampFormatString(const QString &format)
{
    if (_timestampFormatString != format) {
        _timestampFormatString = format;
        // TODO MKO SY emit changed?
    }
}

/******** Caching *******/

QTextCharFormat UiStyle::format(quint64 key) const
{
    return _formats.value(key, QTextCharFormat());
}

QTextCharFormat UiStyle::cachedFormat(quint32 formatType, quint32 messageLabel) const
{
    return _formatCache.value(formatType | ((quint64)messageLabel << 32), QTextCharFormat());
}

void UiStyle::setCachedFormat(const QTextCharFormat &format, quint32 formatType, quint32 messageLabel) const
{
    _formatCache[formatType | ((quint64)messageLabel << 32)] = format;
}

QFontMetricsF *UiStyle::fontMetrics(quint32 ftype, quint32 label) const
{
    // QFontMetricsF is not assignable, so we need to store pointers :/
    quint64 key = ftype | ((quint64)label << 32);

    if (_metricsCache.contains(key))
        return _metricsCache.value(key);

    return (_metricsCache[key] = new QFontMetricsF(format(ftype, label).font()));
}

/******** Generate formats ********/

// NOTE: This and the following functions are intimately tied to the values in FormatType. Don't change this
//       until you _really_ know what you do!
QTextCharFormat UiStyle::format(quint32 ftype, quint32 label_) const
{
    if (ftype == Invalid)
        return QTextCharFormat();

    quint64 label = (quint64)label_ << 32;

    // check if we have exactly this format readily cached already
    QTextCharFormat fmt = cachedFormat(ftype, label_);
    if (fmt.properties().count())
        return fmt;

    mergeFormat(fmt, ftype, label & Q_UINT64_C(0xffff000000000000));

    for (quint64 mask = Q_UINT64_C(0x0000000100000000); mask <= (quint64)Selected << 32; mask <<= 1) {
        if (label & mask)
            mergeFormat(fmt, ftype, mask | Q_UINT64_C(0xffff000000000000));
    }

    setCachedFormat(fmt, ftype, label_);
    return fmt;
}


void UiStyle::mergeFormat(QTextCharFormat &fmt, quint32 ftype, quint64 label) const
{
    mergeSubElementFormat(fmt, ftype & 0x00ff, label);

    if ((ftype & 0xfff00)) { // element format
        for (quint32 mask = 0x00100; mask <= 0x40000; mask <<= 1) {
            if (ftype & mask) {
                mergeSubElementFormat(fmt, ftype & (mask | 0xff), label);
            }
        }
    }

    // URL
    if (ftype & Url)
        mergeSubElementFormat(fmt, ftype & (Url | 0x000000ff), label);
}


// Merge a subelement format into an existing message format
void UiStyle::mergeSubElementFormat(QTextCharFormat &fmt, quint32 ftype, quint64 label) const
{
    quint64 key = ftype | label;
    fmt.merge(format(key & Q_UINT64_C(0x0000ffffffffff00))); // label + subelement
    fmt.merge(format(key & Q_UINT64_C(0x0000ffffffffffff))); // label + subelement + msgtype
    fmt.merge(format(key & Q_UINT64_C(0xffffffffffffff00))); // label + subelement + nickhash
    fmt.merge(format(key & Q_UINT64_C(0xffffffffffffffff))); // label + subelement + nickhash + msgtype
}

UiStyle::FormatType UiStyle::formatType(Message::Type msgType)
{
    switch (msgType) {
    case Message::Plain:
        return PlainMsg;
    case Message::Action:
        return ActionMsg;
    case Message::Nick:
        return NickMsg;
    case Message::Join:
        return JoinMsg;
    case Message::Quit:
        return QuitMsg;
    case Message::Info:
        return InfoMsg;
    case Message::Error:
        return ErrorMsg;
    case Message::DayChange:
        return DayChangeMsg;
    case Message::Invite:
        return InviteMsg;
    }
    //Q_ASSERT(false); // we need to handle all message types
    qWarning() << Q_FUNC_INFO << "Unknown message type:" << msgType;
    return ErrorMsg;
}

UiStyle::FormatType UiStyle::formatType(const QString &code)
{
    if (_formatCodes.contains(code)) return _formatCodes.value(code);
    return Invalid;
}

QString UiStyle::formatCode(FormatType ftype)
{
    return _formatCodes.key(ftype);
}

QList<QTextLayout::FormatRange> UiStyle::toTextLayoutList(const FormatList &formatList, int textLength, quint32 messageLabel) const
{
    QList<QTextLayout::FormatRange> formatRanges;
    QTextLayout::FormatRange range;
    int i = 0;
    for (i = 0; i < formatList.count(); i++) {
        range.format = format(formatList.at(i).second, messageLabel);
        range.start = formatList.at(i).first;
        if (i > 0) formatRanges.last().length = range.start - formatRanges.last().start;
        formatRanges.append(range);
    }
    if (i > 0) formatRanges.last().length = textLength - formatRanges.last().start;
    return formatRanges;
}

// This method expects a well-formatted string, there is no error checking!
// Since we create those ourselves, we should be pretty safe that nobody does something crappy here.
UiStyle::StyledString UiStyle::styleString(const QString &s_, quint32 baseFormat)
{
    QString s = s_;
    if (s.length() > 65535) {
        qWarning() << QString("String too long to be styled: %1").arg(s);
        return StyledString();
    }
    StyledString result;
    result.formatList.append(qMakePair((quint16)0, baseFormat));
    quint32 curfmt = baseFormat;
    int pos = 0; quint16 length = 0;
    for (;;) {
        pos = s.indexOf('%', pos);
        if (pos < 0) break;
        if (s[pos+1] == '%') { // escaped %, we just remove one and continue
            s.remove(pos, 1);
            pos++;
            continue;
        }
        if (s[pos+1] == 'D' && s[pos+2] == 'c') { // color code
            if (s[pos+3] == '-') { // color off
                curfmt &= 0x003fffff;
                length = 4;
            }
            else {
                int color = 10 * s[pos+4].digitValue() + s[pos+5].digitValue();
                //TODO: use 99 as transparent color (re mirc color "standard")
                color &= 0x0f;
                if (s[pos+3] == 'f') {
                    curfmt &= 0xf0ffffff;
                    curfmt |= (quint32)(color << 24) | 0x00400000;
                }
                else {
                    curfmt &= 0x0fffffff;
                    curfmt |= (quint32)(color << 28) | 0x00800000;
                }
                length = 6;
            }
        }
        else if (s[pos+1] == 'O') { // reset formatting
            curfmt &= 0x000000ff; // we keep message type-specific formatting
            length = 2;
        }
        else if (s[pos+1] == 'R') { // reverse
            // TODO: implement reverse formatting

            length = 2;
        }
        else { // all others are toggles
            QString code = QString("%") + s[pos+1];
            if (s[pos+1] == 'D') code += s[pos+2];
            FormatType ftype = formatType(code);
            if (ftype == Invalid) {
                pos++;
                qWarning() << (QString("Invalid format code in string: %1").arg(s));
                continue;
            }
            curfmt ^= ftype;
            length = code.length();
        }
        s.remove(pos, length);
        if (pos == result.formatList.last().first)
            result.formatList.last().second = curfmt;
        else
            result.formatList.append(qMakePair((quint16)pos, curfmt));
    }
    result.plainText = s;
    return result;
}

UiStyle::StyledMessage::StyledMessage(const Message &msg)
    : Message(msg)
{
    if (type() == Message::Plain)
        _senderHash = 0xff;
    else
        _senderHash = 0x00;  // this means we never compute the hash for msgs that aren't plain
}

void UiStyle::StyledMessage::style() const
{
    // TODO MKO SY Mask?
    QString user = sender();

    //QString user = userFromMask(sender());
    QString txt = contents();
    user.replace('%', "%%");     // well, you _can_ have a % in a username apparently... -_-

    // TODO MKO SY Hier ansetzen um die Nachrichtentypen zu dekodieren
    QString t;
    switch (type()) {
    case Message::Plain:
        t = tr("%1").arg(txt); break;
    case Message::Action:
        t = tr("%DN%1%DN %2").arg(user).arg(txt);
        break;
    case Message::Nick:
        if (contents() == user) t = tr("You are now known as %DN%1%DN").arg(txt);
        else t = tr("%DN%1%DN is now known as %DN%2%DN").arg(user, txt);
        break;
    case Message::Join:
        t = tr("%DN%1%DN has joined.").arg(user); break;
    case Message::Quit:
        t = tr("%DN%1%DN has quit").arg(user);
        if (!txt.isEmpty()) t = QString("%1 (%2)").arg(t).arg(txt);
        break;
    case Message::Info:
        t = tr("%1").arg(txt); break;
    case Message::Error:
        t = tr("%1").arg(txt); break;
    case Message::DayChange:
        t = tr("{Day changed to %1}").arg(timestamp().date().toString(Qt::DefaultLocaleLongDate));
        break;
    case Message::Invite:
        //: Invite Message
        t = tr("%1").arg(txt); break;
    default:
        t = tr("[%1]").arg(txt);
    }
    _contents = UiStyle::styleString(t, UiStyle::formatType(type()));
}

const QString &UiStyle::StyledMessage::plainContents() const
{
    if (_contents.plainText.isNull())
        style();

    return _contents.plainText;
}

const UiStyle::FormatList &UiStyle::StyledMessage::contentsFormatList() const
{
    if (_contents.plainText.isNull())
        style();

    return _contents.formatList;
}

QString UiStyle::StyledMessage::decoratedTimestamp() const
{
    return timestamp().toLocalTime().toString(UiStyle::timestampFormatString());
}

QString UiStyle::StyledMessage::plainSender() const
{
    switch (type()) {
    case Message::Plain:
        return sender();
    default:
        return QString();
    }
}

QString UiStyle::StyledMessage::decoratedSender() const
{
    switch (type()) {
    case Message::Plain:
        return tr("%1").arg(plainSender()); break;
    case Message::Action:
        return "*"; break;
    case Message::Nick:
        return "<->"; break;
    case Message::Join:
        return "-->"; break;
    case Message::Quit:
        return "<--"; break;
    case Message::Info:
        return "*"; break;
    case Message::Error:
        return "*"; break;
    case Message::DayChange:
        return "-"; break;
    case Message::Invite:
        return "->"; break;
    default:
        return QString("<%1>").arg(plainSender());
    }
}

// FIXME hardcoded to 16 sender hashes
// TODO MKO SY Was hat es mit den Hashes auf sich?
quint8 UiStyle::StyledMessage::senderHash() const
{
    if (_senderHash != 0xff)
        return _senderHash;

    QString nick = sender().toLower();
    if (!nick.isEmpty()) {
        int chopCount = 0;
        while (chopCount < nick.size() && nick.at(nick.count() - 1 - chopCount) == '_')
            chopCount++;
        if (chopCount < nick.size())
            nick.chop(chopCount);
    }
    quint16 hash = qChecksum(nick.toLatin1().data(), nick.toLatin1().size());
    return (_senderHash = (hash & 0xf) + 1);
}

/***********************************************************************************/

QDataStream &operator<<(QDataStream &out, const UiStyle::FormatList &formatList)
{
    out << formatList.count();
    UiStyle::FormatList::const_iterator it = formatList.begin();
    while (it != formatList.end()) {
        out << (*it).first << (*it).second;
        ++it;
    }
    return out;
}


QDataStream &operator>>(QDataStream &in, UiStyle::FormatList &formatList)
{
    quint16 cnt;
    in >> cnt;
    for (quint16 i = 0; i < cnt; i++) {
        quint16 pos; quint32 ftype;
        in >> pos >> ftype;
        formatList.append(qMakePair((quint16)pos, ftype));
    }
    return in;
}
