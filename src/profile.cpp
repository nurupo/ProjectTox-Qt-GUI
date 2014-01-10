#include "profile.hpp"

Profile::Profile(QString filePath)
{
    pData = data_init_load((char*)filePath.toLocal8Bit().constData());
    if (pData == NULL)
        brokenProfile();
}

Profile::Profile(QString filePath, QString name, QString password)
{ 
    pData = data_init_new((char*)filePath.toLocal8Bit().constData(), (uint8_t*)name.toUtf8().constData(), (uint8_t*)password.toUtf8().constData());
    if (pData == NULL)
        brokenProfile();
}

Profile::~Profile()
{
    if(!pData->locked)
        data_lock(pData);
    data_close(pData);
}

int Profile::unlock(QString password)
{
    return data_unlock(pData, (uint8_t*)password.toUtf8().constData());
}

void Profile::changeName(QString newName)
{
    free(pData->name);
    pData->name = (uint8_t*)malloc(newName.toUtf8().size());
    memcpy(pData->name, newName.toUtf8().constData(), newName.toUtf8().size());
}

QString Profile::getName()
{
    return QString::fromLocal8Bit((const char*)pData->name);
}

QDateTime Profile::getSaveTime()
{
    return QDateTime::fromMSecsSinceEpoch(pData->time_saved);
}

bool Profile::isLocked()
{
    return pData->locked;
}

int Profile::lock()
{
    return data_lock(pData);
}

int Profile::changePassword(QString oldPassword, QString newPassword)
{
    return data_change_key(pData, (uint8_t*)oldPassword.toUtf8().constData(), (uint8_t*)newPassword.toUtf8().constData());
}

size_t Profile::loadData(uint8_t **buffer)
{
    return data_read_messenger(pData, buffer);
}

int Profile::saveData(uint8_t *buffer, size_t length)
{
    return data_write_messenger(pData, buffer, length);
}

int Profile::flush()
{
    return data_flush(pData);
}

void Profile::brokenProfile()
{
    //Create a profile that won't be unlocked. This will probably never be called.
    char tmp[L_tmpnam + 1];
    pData = data_init_new(tmpnam(tmp), (uint8_t*)"Broken Profile!", (uint8_t*)"don'tlogin");
}
