#ifndef MESSAGEMODEL_HPP
#define MESSAGEMODEL_HPP

#include <QAbstractItemModel>
#include <QTimer>
#include "id.hpp"
#include "message.hpp"
#include "messagemodelitem.hpp"

class MessageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum MessageModelRole {
        DisplayRole = Qt::DisplayRole,
        EditRole = Qt::EditRole,
        BackgroundRole = Qt::BackgroundRole,
        MessageRole = Qt::UserRole,
        MsgIdRole,
        TypeRole,
        FlagsRole,
        TimestampRole,
        FormatRole,
        ColumnTypeRole,
        UserRole,
        WrapListRole,
        MsgLabelRole,
        SelectedBackgroundRole
        };

    enum ColumnType {
        SenderColumn,
        ContentsColumn,
        TimestampColumn
        };

    typedef MessageModelItem::Word Word;
    typedef MessageModelItem::WrapList WrapList;

    explicit MessageModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    inline QModelIndex parent(const QModelIndex &) const { return QModelIndex(); }
    inline int rowCount(const QModelIndex &parent = QModelIndex()) const { return parent.isValid() ? 0 : messageCount(); }
    inline int columnCount(const QModelIndex & /*parent*/ = QModelIndex()) const { return 3; }

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertMessage(const Message &);
    void insertMessages(const QList<Message> &);
    void insertNewMessage(const QString& content, const QString& sender, Message::Type type, Message::Flag flag = Message::None);

    inline const MessageModelItem *messageItemAt(int i) const { return &_messageList[i]; }

    void clear();

signals:

public slots:
    void insertErrorMessage(const QString &errorString);

protected:
    inline int messageCount() const { return _messageList.count(); }
    inline bool messagesIsEmpty() const { return _messageList.isEmpty(); }
    inline MessageModelItem *messageItemAt(int i) { return &_messageList[i]; }
    inline const MessageModelItem *firstMessageItem() const { return &_messageList.first(); }
    inline MessageModelItem *firstMessageItem() { return &_messageList.first(); }
    inline const MessageModelItem *lastMessageItem() const { return &_messageList.last(); }
    inline MessageModelItem *lastMessageItem() { return &_messageList.last(); }
    inline void insertMessage__(int pos, const Message &msg) { _messageList.insert(pos, MessageModelItem(msg)); }
    void insertMessages__(int pos, const QList<Message> &messages);
    inline void removeMessageAt(int i) { _messageList.removeAt(i); }
    inline void removeAllMessages() { _messageList.clear(); }
    Message takeMessageAt(int i);

    void customEvent(QEvent *event);

private slots:
    void changeOfDay();

private:
    void insertMessageGroup(const QList<Message> &);
    int insertMessagesGracefully(const QList<Message> &); // inserts as many contiguous msgs as possible. returns numer of inserted msgs.
    int indexForId(MsgId);

    QList<Message> _messageBuffer;
    QTimer _dayChangeTimer;
    QDateTime _nextDayChange;

    QList<MessageModelItem> _messageList;

};

QDataStream &operator<<(QDataStream &out, const MessageModel::WrapList wplist);
QDataStream &operator>>(QDataStream &in, MessageModel::WrapList &wplist);

Q_DECLARE_METATYPE(MessageModel::WrapList)

#endif // MESSAGEMODEL_HPP
