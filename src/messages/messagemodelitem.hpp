#ifndef MESSAGEMODELITEM_HPP
#define MESSAGEMODELITEM_HPP

#include "message.hpp"
#include <QObject>

class MessageModelItem
{
    Q_DECLARE_TR_FUNCTIONS(MessageModelItem)
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

    inline const Message &message() const { return mMsg; }
    inline const QDateTime &timestamp() const { return mMsg.timestamp(); }
    inline const MsgId &msgId() const { return mMsg.msgId(); }
    inline Message::Type msgType() const { return mMsg.type(); }
    inline Message::Flags msgFlags() const { return mMsg.flags(); }

    // For sorting
    bool operator<(const MessageModelItem &) const;
    bool operator==(const MessageModelItem &) const;
    bool operator>(const MessageModelItem &) const;
    static bool lessThan(const MessageModelItem *m1, const MessageModelItem *m2);

private:
    QVariant timestampData(int role) const;
    QVariant senderData(int role) const;
    QVariant contentsData(int role) const;

    Message mMsg;
};


QDebug operator<<(QDebug dbg, const MessageModelItem &msgItem);

#endif // MESSAGEMODELITEM_HPP
