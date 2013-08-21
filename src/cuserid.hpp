#ifndef CUSERID_HPP
#define CUSERID_HPP

#include <QString>

class CUserId
{
public:
    explicit CUserId(const QString& userId);
    ~CUserId();

    uint8_t* data();
    uint16_t size();

    static QString toString(const uint8_t* cUserId/*, uint16_t cUserIdSize*/);

private:
    uint8_t* cUserId;
    uint16_t cUserIdSize;

    static uint16_t fromString(const QString& userId, uint8_t* cUserId);
};

#endif // CUSERID_HPP
