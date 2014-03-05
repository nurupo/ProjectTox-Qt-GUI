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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QCoreApplication>
#include <QDateTime>

#include "id.hpp"

class Message
{
    Q_DECLARE_TR_FUNCTIONS(Message)

public:
    /** The different types a message can have for display */
    enum Type {
        Plain     = 0x00001,
        Action    = 0x00004,
        Nick      = 0x00008,
        Join      = 0x00020,
        Quit      = 0x00080,
        Info      = 0x00800,
        Error     = 0x01000,
        DayChange = 0x02000,
        Invite    = 0x20000,
    };

    // DO NOT CHANGE without knowing what you do, some of these flags are stored in the database
    enum Flag {
        None = 0x00,
        Self = 0x01,
        Highlight = 0x02,
        Redirected = 0x04,
        ServerMsg = 0x08,
        Backlog = 0x80
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    Message(Message::Type type = Plain, const QString &contents = "", const QString &sender = "", Flags flags = None);
    Message(const QDateTime &ts, Type type = Plain, const QString &contents = "", const QString &sender = "", Flags flags = None);

    inline static Message changeOfDay(const QDateTime &day) { return Message(day, DayChange); }
    inline const MsgId &msgId() const { return _msgId; }
    inline void setMsgId(MsgId id) { _msgId = id; }
    inline const QString &contents() const { return _contents; }
    inline const QString &sender() const { return _sender; }
    inline Type type() const { return _type; }
    inline Flags flags() const { return _flags; }
    inline void setFlags(Flags flags) { _flags = flags; }
    inline const QDateTime &timestamp() const { return _timestamp; }
    inline bool isValid() const { return _msgId.isValid(); }

    inline bool operator<(const Message &other) const { return _msgId < other._msgId; }

private:
    QDateTime _timestamp;
    MsgId _msgId;
    QString _contents;
    QString _sender;
    Type _type;
    Flags _flags;

    friend QDataStream &operator>>(QDataStream &in, Message &msg);
};

typedef QList<Message> MessageList;

QDataStream &operator<<(QDataStream &out, const Message &msg);
QDataStream &operator>>(QDataStream &in, Message &msg);
QDebug operator<<(QDebug dbg, const Message &msg);

Q_DECLARE_METATYPE(Message)
Q_DECLARE_OPERATORS_FOR_FLAGS(Message::Flags)

#endif // MESSAGE_HPP
