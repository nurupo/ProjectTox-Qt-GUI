#ifndef MESSAGEMODELITEM_HPP
#define MESSAGEMODELITEM_HPP

#include "message.hpp"

class MessageModelItem
{
public:
    //! Creates a MessageModelItem from a Message object.
    /** This baseclass implementation takes care of all Message data *except* the stylable strings.
     *  Subclasses need to provide Qt::DisplayRole at least, which should describe the plaintext
     *  strings without formattings (e.g. for searching purposes).
     */
    MessageModelItem(const Message &msg);
    inline virtual ~MessageModelItem() {}

    QVariant data(int column, int role) const;
    bool setData(int column, const QVariant &value, int role);

    inline const Message &message() const { return _styledMsg; }
    inline const QDateTime &timestamp() const { return _styledMsg.timestamp(); }
    inline const MsgId &msgId() const { return _styledMsg.msgId(); }
    inline Message::Type msgType() const { return _styledMsg.type(); }
    inline Message::Flags msgFlags() const { return _styledMsg.flags(); }

    // For sorting
    bool operator<(const MessageModelItem &) const;
    bool operator==(const MessageModelItem &) const;
    bool operator>(const MessageModelItem &) const;
    static bool lessThan(const MessageModelItem *m1, const MessageModelItem *m2);

    inline void invalidateWrapList() { _wrapList.clear(); }

    /// Used to store information about words to be used for wrapping
    struct Word {
        quint16 start;
        qreal endX;
        qreal width;
        qreal trailing;
    };
    typedef QVector<Word> WrapList;

private:
    QVariant timestampData(int role) const;
    QVariant senderData(int role) const;
    QVariant contentsData(int role) const;

    void computeWrapList() const;

    mutable WrapList _wrapList;
    Message _styledMsg;

    static unsigned char *TextBoundaryFinderBuffer;
    static int TextBoundaryFinderBufferSize;
};


QDebug operator<<(QDebug dbg, const MessageModelItem &msgItem);

#endif // MESSAGEMODELITEM_HPP
