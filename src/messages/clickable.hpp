#ifndef CLICKABLE_HPP
#define CLICKABLE_HPP

#include "id.hpp"

class QModelIndex;

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

#endif // CLICKABLE_HPP
