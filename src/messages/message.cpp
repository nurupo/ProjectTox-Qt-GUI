#include "message.hpp"

#include <QDataStream>

Message::Message(Type type, const QString &contents, const QString &sender, Flags flags) :
    _timestamp(QDateTime::currentDateTime().toUTC()),
    _contents(contents),
    _sender(sender),
    _type(type),
    _flags(flags)
{
}

Message::Message(const QDateTime &ts, Type type, const QString &contents, const QString &sender, Flags flags) :
    _timestamp(ts),
    _contents(contents),
    _sender(sender),
    _type(type),
    _flags(flags)
{
}

QDataStream &operator<<(QDataStream &out, const Message &msg)
{
    out << msg.msgId() << (quint32)msg.timestamp().toTime_t() << (quint32)msg.type() << (quint8)msg.flags()
    << msg.sender().toUtf8() << msg.contents().toUtf8();
    return out;
}

QDataStream &operator>>(QDataStream &in, Message &msg)
{
    quint8 f;
    quint32 t;
    quint32 ts;
    QByteArray s, m;
    in >> msg._msgId >> ts >> t >> f >> s >> m;
    msg._type = (Message::Type)t;
    msg._flags = (Message::Flags)f;
    msg._timestamp = QDateTime::fromTime_t(ts);
    msg._sender = QString::fromUtf8(s);
    msg._contents = QString::fromUtf8(m);
    return in;
}

QDebug operator<<(QDebug dbg, const Message &msg)
{
    dbg.nospace() << qPrintable(QString("Message(MsgId:")) << msg.msgId()
    << qPrintable(QString(",")) << msg.timestamp()
    << qPrintable(QString(", Type:")) << msg.type()
    << qPrintable(QString(", Flags:")) << msg.flags() << qPrintable(QString(")"))
    << msg.sender() << ":" << msg.contents();
    return dbg;
}
