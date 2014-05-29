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

#ifndef CLICKABLE_HPP
#define CLICKABLE_HPP

#include "id.hpp"

class QModelIndex;
class ChatItem;

class Clickable
{
public:
    enum Type {
        Invalid = -1,
        Url     = 0,
        Nick    = 2
    };

    explicit Clickable(Type type = Invalid, quint16 start = 0, quint16 length = 0);

    inline Type type() const { return _type; }
    inline quint16 start() const { return _start; }
    inline quint16 length() const { return _length; }

    inline bool isValid() const { return _type != Invalid; }

    void activate(const QString &text) const;

private:
    Type    _type;
    quint16 _start;
    quint16 _length;
};

class ClickableList : public QList<Clickable>
{
public:
    static ClickableList fromString(const QString &str);

    Clickable atCursorPos(int idx);
};

// ============================================================================

class Highlight
{
public:
    enum Type {
        Invalid = -1,
        Found   = 0,
        Current = 1
    };

    explicit Highlight(Type type = Invalid, ChatItem *item = nullptr, quint16 start = 0, quint16 length = 0);

    inline Type type() const { return mType; }
    inline quint16 start() const { return mStart; }
    inline void setStart(quint16 s) { mStart = s; }
    inline quint16 length() const { return mLength; }
    inline void setLength(quint16 l) { mLength = l; }

    inline bool isValid() const { return mType != Invalid; }

    inline void setCurrent(bool c = true) { if(c) mType = Current; else mType = Found; }
    inline void unsetCurrent() { mType = Found; }


    inline ChatItem *item() { return mItem; }
    inline void setItem(ChatItem *value) { mItem = value; }

private:
    Type      mType;
    ChatItem *mItem;
    quint16   mStart;
    quint16   mLength;
};

#endif // CLICKABLE_HPP
