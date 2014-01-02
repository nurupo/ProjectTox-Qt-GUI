#ifndef PROFILE_H
#define PROFILE_H

#include <tox.h>
#include <util.h>
#include <sodium.h>

extern "C" {
#include <scrypt-jane.h>
}

#include <QObject>
#include <QFile>
#include <QDateTime>

/* Profile Save Format
 * ==============
 *
 * bytes    name        type        purpose
 * ----------------------------------------
 * -- block one [unencrypted] --
 * 4        magic       uint8       magic,6c:69:62:65 "libe"
 * 8        saved       uint64      timestamp of when the profile was last used
 * 2        namelen     uint16      length of name
 * varies   name        uint8       name of profile, UTF-8
 * 12       scryptvars  uint32      N,r,p variables for scrypt - in this order
 * 24       salt        uint8       the salt for scrypt
 * 24       nonce       uint8       the nonce for nacl
 * 8        blocklen    uint64      the length of the encrypted block
 * -- block two [encrypted] --
 * 32       0           uint8       crypto_secretbox_ZEROBYTES
 * 4        magic       uint8       magic,72:74:61:73 "rtas"
 * varies   profile     uint8       the messenger data - this goes to tox_load()
 */

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
    size_t loadData(uint8_t *buffer);

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
    int loadFile();
    int saveFile();

    //The profile's save path & encryption status.
    QString pPath;
    bool pLocked = true;

    uint8_t encryptedKey[crypto_secretbox_KEYBYTES], //Sodium encrypt key, 32 bytes.
            nonce[crypto_secretbox_NONCEBYTES], //Sodium nonce, 24 bytes.
            salt[24]; //Scrypt salt, 24 bytes.

    /* These values were sourced from here: https://www.tarsnap.com/scrypt/scrypt-slides.pdf
     * Percival recommends these values for interactive logins - we can always adjust them later
     * for increased difficulty.
     */
    uint32_t scryptN = 15, scryptR = 8, scryptP = 1;

    //The profile's name and last save time.
    QString pName;
    QDateTime pSavedTime;

    //The encrypted block's file offset and length.
    size_t blockTwoOffset;
    uint64_t blockTwoLength;

    //The unencrypted messenger data.
    uint8_t *pData = nullptr;
    size_t pDataLength;
};

#endif // PROFILE_H
