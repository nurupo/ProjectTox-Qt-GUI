#include "messagemodelitem.hpp"
#include "messagemodel.hpp"
#include "Settings/settings.hpp"

#include <QApplication>

MessageModelItem::MessageModelItem(const Message &msg) :
    mMsg(msg)
{
    if (!msg.sender().contains('!'))
        mMsg.setFlags(msg.flags() |= Message::ServerMsg);
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
        mMsg.setFlags((Message::Flags)value.toUInt());
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
        return mMsg.timestamp().toLocalTime().toString(Settings::getInstance().getTimestampFormat());
    case MessageModel::EditRole:
        return mMsg.timestamp();
    case MessageModel::ForegroundRole:
        return QVariant::fromValue<QBrush>(QApplication::palette().mid());
    }
    return QVariant();
}

QVariant MessageModelItem::senderData(int role) const
{
    switch (role) {
    case MessageModel::DisplayRole:
        switch (mMsg.type()) {
        case Message::Plain:
            return mMsg.sender();
        case Message::Action:
            return "*";
        case Message::Nick:
            return "<->";
        case Message::Join:
            return "-->";
        case Message::Quit:
            return "<--";
        case Message::Info:
            return "*";
        case Message::Error:
            return "*";
        case Message::DayChange:
            return "---";
        case Message::Invite:
            return "->";
        default:
            return mMsg.sender();
        }
    case MessageModel::EditRole:
        return mMsg.sender();
    case MessageModel::ForegroundRole:
        if((mMsg.type() == Message::Plain) && (mMsg.flags().testFlag(Message::Self)))
            return QVariant::fromValue<QBrush>(QApplication::palette().mid());
        else
            return QVariant::fromValue<QBrush>(foreground(mMsg.type()));
    }
    return QVariant();
}

QVariant MessageModelItem::contentsData(int role) const
{
    switch (role) {
    case MessageModel::DisplayRole:
    case MessageModel::EditRole:
        switch (mMsg.type()) {
        case Message::Plain:
            return mMsg.contents();
        case Message::Action:
            return QString("%1 %2").arg(mMsg.sender(), mMsg.contents());
        case Message::Nick:
            if (mMsg.contents() == mMsg.sender())
                return tr("You are now known as %1").arg(mMsg.contents());
            else
                return tr("%1 is now known as %2").arg(mMsg.sender(), mMsg.contents());
        case Message::Join:
            return tr("%1 has joined.").arg(mMsg.sender()); break;
        case Message::Quit:
            return tr("%1 has gone.").arg(mMsg.sender());
        case Message::Info:
        case Message::Error:
            return mMsg.contents();
        case Message::DayChange:
            return tr("{Day changed to %1}").arg(mMsg.timestamp().date().toString(Qt::DefaultLocaleLongDate));
        case Message::Invite:
        default:
            return mMsg.contents();
        }
    case MessageModel::ForegroundRole:
        return QVariant::fromValue<QBrush>(foreground(mMsg.type()));
    }
    return QVariant();
}

QBrush MessageModelItem::foreground(Message::Type type) const
{
    switch (type) {
    case Message::Plain:
        return QApplication::palette().text();
    case Message::Action:
        return QBrush("#4E9A06");
    case Message::Nick:
        return QBrush("#204A87");
    case Message::Join:
    case Message::Quit:
    case Message::Info:
    case Message::Error:
        return QApplication::palette().text();
    case Message::DayChange:
        return QBrush("#AD7FA8");
    case Message::Invite:
    default:
        return QApplication::palette().text();
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
