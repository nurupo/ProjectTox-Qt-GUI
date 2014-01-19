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
    qint32 id;

public:
    inline SignedId(int _id = 0) { id = _id; }
    inline qint32 toInt() const { return id; }
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

inline QDataStream &operator<<(QDataStream &out, const SignedId &signedId) { out << signedId.toInt(); return out; }
inline QDataStream &operator>>(QDataStream &in, SignedId &signedId) { in >> signedId.id; return in; }
inline QTextStream &operator<<(QTextStream &out, const SignedId &signedId) { out << QString::number(signedId.toInt()); return out; }
inline QDebug operator<<(QDebug dbg, const SignedId &signedId) { dbg.space() << signedId.toInt(); return dbg; }
inline uint qHash(const SignedId &id) { return qHash(id.toInt()); }

struct MsgId : public SignedId {
    inline MsgId(int _id = 0) : SignedId(_id) {}
    //inline operator QVariant() const { return QVariant::fromValue<MsgId>(*this); }
};

Q_DECLARE_METATYPE(MsgId)

typedef QList<MsgId> MsgIdList;

#endif // ID_HPP
