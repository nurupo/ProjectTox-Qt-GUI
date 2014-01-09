#ifndef PROFILE_H
#define PROFILE_H

#include <tox.h>
#include <util.h>
#include <sodium.h>

extern "C" {
#include "tox_data.h"
}

#include <QObject>
#include <QFile>
#include <QDateTime>

class Profile : public QObject
{
    Q_OBJECT
public:
    /* See tox_data.h for full documentation on function return values & behavior. */

    /* Loads a profile from the given path.
     * Throws an exception (0) if the profile can't be loaded.
     */
    Profile(QString filePath);
    /* Creates a new profile.
     * This profile isn't saved to disk until lock() or save() are called.
     */
    Profile(QString filePath, QString name, QString password);
    ~Profile();

    int unlock(QString password);

    /* Changes the saved name for the profile. */
    void changeName(QString newName);
    /* Returns the profile's name. */
    QString getName();
    /* Returns the last date that the profile was saved. */
    QDateTime getSaveTime();

    /* Checks if the profile is locked. */
    bool isLocked();

    /* ------------- REQUIRES UNLOCKING ------------- */

    int lock();
    int changePassword(QString oldPassword, QString newPassword);
    size_t loadData(uint8_t **buffer);
    int saveData(uint8_t *buffer, size_t length);
    int flush();

private:
    tox_data *pData;
};

#endif // PROFILE_H
