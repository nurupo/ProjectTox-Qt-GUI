/*
    Copyright (C) 2005-2014 by the Quassel Project <devel@quassel-irc.org>
                  2014 by Martin Kröll <technikschlumpf@web.de>

    This file is part of Tox Qt GUI.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

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
