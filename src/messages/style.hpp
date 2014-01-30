#ifndef STYLE_HPP
#define STYLE_HPP

#include <QObject>
#include <QTextLayout>
#include "message.hpp"

class QTextCharFormat;
class QFontMetricsF;

class UiStyle : public QObject
{
    Q_OBJECT

public:
    static UiStyle &getInstance();
    virtual ~UiStyle();

    typedef QList<QPair<quint16, quint32> > FormatList;

    //! This enumerates the possible formats a text element may have. */
    /** These formats are ordered on increasing importance, in cases where a given property is specified
     *  by multiple active formats.
     *  \NOTE: Do not change/add values here without also adapting the relevant
     *         methods in this class (in particular mergedFormat())!
     *         Also, we _do_ rely on certain properties of these values in styleString() and friends!
     */
    enum FormatType {
        Base            = 0x00000000,
        Invalid         = 0xffffffff,

        // Message Formats (mutually exclusive!)
        PlainMsg        = 0x00000001,
        ActionMsg       = 0x00000003,
        NickMsg         = 0x00000004,
        JoinMsg         = 0x00000006,
        QuitMsg         = 0x00000008,
        InfoMsg         = 0x0000000c,
        ErrorMsg        = 0x0000000d,
        DayChangeMsg    = 0x0000000e,
        InviteMsg       = 0x00000030,

        // Standard Formats
        Bold            = 0x00000100,
        Italic          = 0x00000200,
        Underline       = 0x00000400,
        Reverse         = 0x00000800,

        // Individual parts of a message
        Timestamp       = 0x00001000,
        Sender          = 0x00002000,
        Contents        = 0x00004000,
        Nick            = 0x00008000,
        Hostmask        = 0x00010000,
        ChannelName     = 0x00020000,
        ModeFlags       = 0x00040000,

        // URL is special, we want that to take precedence over the rest...
        Url             = 0x00080000
    };

    enum MessageLabel {
        Default         = 0x00000000,
        OwnMsg          = 0x00000001,
        Highlight       = 0x00000002,
        Selected        = 0x00000004 // must be last!
    };

    enum ColorRole {
        MarkerLine,
        NumRoles // must be last!
    };

    struct StyledString {
        QString plainText;
        FormatList formatList; // starting pos, ftypes
    };

    class StyledMessage;

    static FormatType formatType(Message::Type msgType);
    static StyledString styleString(const QString &string, quint32 baseFormat = Base);
    static inline QString timestampFormatString() { return _timestampFormatString; }

    QTextCharFormat format(quint32 formatType, quint32 messageLabel) const;
    QFontMetricsF *fontMetrics(quint32 formatType, quint32 messageLabel) const;

    QList<QTextLayout::FormatRange> toTextLayoutList(const FormatList &, int textLength, quint32 messageLabel) const;

    inline const QBrush &brush(ColorRole role) const { return _uiStylePalette.at((int)role); }
    inline void setBrush(ColorRole role, const QBrush &brush) { _uiStylePalette[(int)role] = brush; }

    QVariant bufferViewItemData(const QModelIndex &networkModelIndex, int role) const;
    QVariant nickViewItemData(const QModelIndex &networkModelIndex, int role) const;

signals:
    void changed();

protected:
    QTextCharFormat format(quint64 key) const;
    QTextCharFormat cachedFormat(quint32 formatType, quint32 messageLabel) const;
    void setCachedFormat(const QTextCharFormat &format, quint32 formatType, quint32 messageLabel) const;
    void mergeFormat(QTextCharFormat &format, quint32 formatType, quint64 messageLabel) const;
    void mergeSubElementFormat(QTextCharFormat &format, quint32 formatType, quint64 messageLabel) const;

    static FormatType formatType(const QString &code);
    static QString formatCode(FormatType);
    static void setTimestampFormatString(const QString &format);

private:
    UiStyle(QObject *parent = 0);

    void createFormat(FormatType type = Base, MessageLabel label = Default, QBrush foreground = QBrush(), QBrush background = QBrush());

    QVector<QBrush> _uiStylePalette;
    QHash<quint64, QTextCharFormat> _formats;
    mutable QHash<quint64, QTextCharFormat> _formatCache;
    mutable QHash<quint64, QFontMetricsF *> _metricsCache;
    QHash<quint32, QTextCharFormat> _listItemFormats;
    static QHash<QString, FormatType> _formatCodes;
    static QString _timestampFormatString;
};



class UiStyle::StyledMessage : public Message
{
    Q_DECLARE_TR_FUNCTIONS(UiStyle::StyledMessage)

public:
    explicit StyledMessage(const Message &message);

    QString decoratedTimestamp() const;
    QString plainSender() const;           //!< Nickname (no decorations) for Plain and Notice, empty else
    QString decoratedSender() const;
    const QString &plainContents() const;

    const FormatList &contentsFormatList() const;

    quint8 senderHash() const;

protected:
    void style() const;

private:
    mutable StyledString _contents;
    mutable quint8 _senderHash;
};


QDataStream &operator<<(QDataStream &out, const UiStyle::FormatList &formatList);
QDataStream &operator>>(QDataStream &in, UiStyle::FormatList &formatList);

Q_DECLARE_METATYPE(UiStyle::FormatList)

#endif // STYLE_HPP
