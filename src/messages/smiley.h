#ifndef SMILEY_H
#define SMILEY_H

#include <QString>
#include <QList>

class Smiley
{
public:
    enum Type {
        Invalid = 0,
        Pixmap  = 1,
        Emoji   = 2
    };

    explicit Smiley(const QString &text = "", const QString &graphics = "", int start = 0, Type type = Invalid);

    inline Type type() const { return mType; }
    inline int start() const { return mStart; }
    inline int textLength() const { return mTextLength; }
    inline bool isValid() const { return mType != Invalid; }
    inline QString text() const { return mText; }

    /*! Returns pixmap URL or emoji character */
    inline QString graphics() const { return mGraphics; }

private:
    Type mType;
    int  mStart;
    int  mTextLength;
    QString mText;
    QString mGraphics; // pixmap URL or emoji character
};

class SmileyList : public QList<Smiley>
{
public:
    static SmileyList smilify(QString &text);

    Smiley atCursorPos(int idx);
};

#endif // SMILEY_H
