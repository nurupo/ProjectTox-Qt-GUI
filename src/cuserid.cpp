#include "cuserid.hpp"

#include <Messenger.h>

CUserId::CUserId(const QString &userId)
{
    cUserId = new uint8_t[FRIEND_ADDRESS_SIZE];
    cUserIdSize = fromString(userId, cUserId);
}

CUserId::~CUserId()
{
    delete[] cUserId;
}

uint8_t* CUserId::data()
{
    return cUserId;
}

uint16_t CUserId::size()
{
    return cUserIdSize;
}

QString CUserId::toString(const uint8_t* cUserId/*, uint16_t cUserIdSize*/)
{
    return QString(QByteArray(reinterpret_cast<const char*>(cUserId), FRIEND_ADDRESS_SIZE).toHex()).toUpper();
}

uint16_t CUserId::fromString(const QString& userId, uint8_t* cUserId)
{
    QByteArray arr = QByteArray::fromHex(userId.toLower().toLatin1());
    memcpy(cUserId, reinterpret_cast<uint8_t*>(arr.data()), arr.size());
    return arr.size();
}

