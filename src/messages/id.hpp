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

#ifndef ID_HPP
#define ID_HPP

#include <QString>
#include <QVariant>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>

class SignedId
{
protected:
    qint64 id;

public:
    inline SignedId(qint64 _id = 0) { id = _id; }
    inline qint64 toLong() const { return id; }
    inline bool isValid() const { return id > 0; }

    inline bool operator==(const SignedId &other) const { return id == other.id; }
    inline bool operator!=(const SignedId &other) const { return id != other.id; }
    inline bool operator<(const SignedId &other) const { return id < other.id; }
    inline bool operator<=(const SignedId &other) const { return id <= other.id; }
    inline bool operator>(const SignedId &other) const { return id > other.id; }
    inline bool operator>=(const SignedId &other) const { return id >= other.id; }
    inline bool operator==(int i) const { return id == i; }
    inline bool operator!=(int i) const { return id != i; }
    inline bool operator<(int i) const { return id < i; }
    inline bool operator>(int i) const { return id > i; }
    inline bool operator<=(int i) const { return id <= i; }

    inline SignedId operator++(int) { id++; return *this; }
    //inline operator int() const { return toInt(); } // no automatic conversion!

    friend QDataStream &operator>>(QDataStream &in, SignedId &signedId);
};

inline QDataStream &operator<<(QDataStream &out, const SignedId &signedId) { out << signedId.toLong(); return out; }
inline QDataStream &operator>>(QDataStream &in, SignedId &signedId) { in >> signedId.id; return in; }
inline QTextStream &operator<<(QTextStream &out, const SignedId &signedId) { out << QString::number(signedId.toLong()); return out; }
inline QDebug operator<<(QDebug dbg, const SignedId &signedId) { dbg.space() << signedId.toLong(); return dbg; }
inline uint qHash(const SignedId &id) { return qHash(id.toLong()); }

struct MsgId : public SignedId {
    inline MsgId(qint64 _id = 0) : SignedId(_id) {}
    //inline operator QVariant() const { return QVariant::fromValue<MsgId>(*this); }
};

Q_DECLARE_METATYPE(MsgId)

typedef QList<MsgId> MsgIdList;

#endif // ID_HPP
