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
    /* Loads a profile from the given path.
     * Throws an exception (0) if the profile can't be loaded.
     */
    Profile(QString filePath);
    /* Creates a new profile.
     * This profile isn't saved to disk until lock() or save() are called.
     */
    Profile(QString filePath, QString name, QString password);
    ~Profile();

    /* Attempts to decrypt the messenger.
     *
     * returns 0 if success
     */
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

    /* Locks the profile, requiring an unlock() to again access.
     * This function also flushes data to disk.
     *
     * returns 0 if success
     */
    int lock();

    /* Changes the password for the profile.
     * This does not lock the profile
     *
     * returns 0 if success.
     */
    int changePassword(QString oldPassword, QString newPassword);

    /* Loads the messenger into the given buffer.
     *
     * returns the size of the buffer - 0, if unsuccessful
     */
    size_t loadData(uint8_t **buffer);

    /* Saves the given messenger.
     * This does not lock the profile.
     *
     * returns 0 if successful
     */
    int saveData(uint8_t *buffer, size_t length);

    /* Saves the current data to disk.
     * This does not lock the profile.
     *
     * returns 0 if success
     */
    int flush();

private:
    tox_data *pData;
};

#endif // PROFILE_H
