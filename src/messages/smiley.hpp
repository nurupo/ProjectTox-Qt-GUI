/*
    Copyright (C) 2014 by Martin Kröll <technikschlumpf@web.de>

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

#ifndef SMILEY_H
#define SMILEY_H

#include <QString>
#include <QList>
#include <QFont>

class Smiley
{
public:
    enum Type {
        Invalid = 0,
        Pixmap  = 1,
        Emoji   = 2
    };

    explicit Smiley(const QString &text = "", const QString &graphics = "", int start = 0, int smileyfiedStart = 0, Type type = Invalid);

    inline Type type() const { return mType; }
    inline int start() const { return mStart; }
    inline int smileyfiedStart() const { return mSmiletfiedStart; }
    inline int textLength() const { return mTextLength; }
    inline bool isValid() const { return mType != Invalid; }
    inline QString text() const { return mText; }

    /*! Returns pixmap URL or emoji character */
    inline QString graphics() const { return mGraphics; }

    //Emoji specific
    inline QFont emojiFont() const { return mEmojiFont; }
    inline void setEmojiFont(const QFont &value) { mEmojiFont = value; }

private:
    Type mType;
    int  mStart;
    int  mSmiletfiedStart; // start pos in smileyfied QTextDocument
    int  mTextLength;
    QString mText;
    QString mGraphics; // pixmap URL or emoji character
    QFont   mEmojiFont;
};

class SmileyList : public QList<Smiley>
{
public:
    static SmileyList fromText(QString text);

    //Smiley atCursorPos(int idx);
};

#endif // SMILEY_H
