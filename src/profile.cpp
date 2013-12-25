#include "profile.hpp"

Profile::Profile(QString filePath)
{
    pPath = filePath;
    //pTox = tox_new(0);
    if (loadFile() != 0)
        throw 0;
}

Profile::Profile(QString filePath, QString name, QString password)
{
    pPath = filePath;
    pName = name;
    //pTox = tox_new(0);

    //if (pTox == nullptr)
        //throw 0;

    //derive new key for saving
    randombytes(salt, 24);
    randombytes(nonce, 24);
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);
    pLocked = false;
}

Profile::~Profile()
{
    if(!pLocked)
        lock();
}

int Profile::unlock(QString password)
{
    if(!pLocked)
        return 0;

    //load encrypted block
    QFile file(pPath);
    file.open(QFile::ReadOnly);
    uint8_t *rawFile = file.map(0, file.size());

    uint8_t blockTwoEncrypted[blockTwoLength], blockTwoPlaintext[blockTwoLength];
    memcpy(blockTwoEncrypted, rawFile + blockTwoOffset, blockTwoLength);

    file.unmap(rawFile);
    file.close();

    //derive key from file
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    //decrypt block
    if(crypto_secretbox_open(blockTwoPlaintext,blockTwoEncrypted,blockTwoLength,nonce,encryptedKey) != 0)
        return -1;

    //check magic
    char magic[4];
    memcpy(magic, blockTwoPlaintext + 32, 4);
    if(memcmp(magic,&"rtas",4) != 0)
        return -1;

    //load for future use
    if(pData != nullptr)
        free(pData);
    pDataLength = blockTwoLength - 36;
    pData = (uint8_t*)malloc(pDataLength);
    memcpy(pData, blockTwoPlaintext + 36, pDataLength);

    //create this profile's tox instance
    //tox_load(pTox, messenger, blockTwoLength - 36);

    //check against loaded scrypt values being too small
    if(scryptN < 15)
        scryptN = 15;
    if(scryptR < 8)
        scryptR = 8;

    /* Generate a new key for future saving.
     * This somewhat more secure than keeping the user password around in plaintext in
     * the ram (keeping in mind, of course, that it's already game-over if the client is
     * compromised). Still though - it's the thought that counts.
     */
    randombytes(salt, 24);
    randombytes(nonce, 24);
    scrypt((const uint8_t*)password.toLocal8Bit().constData(), password.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    memset(blockTwoPlaintext, 0, blockTwoLength);

    pLocked = false;

    return 0;
}

void Profile::changeName(QString newName)
{
    pName = newName;
}

QString Profile::getName()
{
    return pName;
}

QDateTime Profile::getSaveTime()
{
    return pSavedTime;
}

bool Profile::isLocked()
{
    return pLocked;
}


int Profile::lock()
{
    if(pLocked)
        return -1;

    saveFile();

    //tox_kill(pTox);
    //pTox = nullptr;

    memset(pData,0,pDataLength);
    memset(encryptedKey,0,32);

    pLocked = true;
    return 0;
}

int Profile::changePassword(QString oldPassword, QString newPassword)
{
    if(pLocked)
        if(unlock(oldPassword) != 0)
            return -1;

    uint8_t oldKey[32];
    scrypt((const uint8_t*)oldPassword.toLocal8Bit().constData(), oldPassword.length(), salt, 24, scryptN, scryptR, scryptP, oldKey, 32);

    /* Check to see if keys match.
     * Although there's no cryptographic need to do so, it ensures that whomever's using
     * the profile should have access to it, rather than having stumbled upon an unlocked
     * computer.
     */
    if(strcmp((const char*)oldKey,(const char*)encryptedKey) != 0)
        return -1;

    scrypt((const uint8_t*)newPassword.toLocal8Bit().constData(), newPassword.length(), salt, 24, scryptN, scryptR, scryptP, encryptedKey, 32);

    return 0;
}

size_t Profile::loadData(uint8_t *buffer)
{
    if(pLocked)
        return -1;

    buffer = (uint8_t*)malloc(pDataLength);
    memcpy(buffer, pData, pDataLength);
    return pDataLength;
}

int Profile::saveData(uint8_t *buffer, size_t length)
{
    if(pLocked)
        return -1;

    if(pData != nullptr)
        free(pData);

    pData = (uint8_t*)malloc(length);
    memcpy(pData, buffer, length);
    saveFile();

    return 0;
}

int Profile::flush()
{
    if(pLocked)
        return -1;

    saveFile();
    return 0;
}

int Profile::loadFile()
{
    QFile file(pPath);
    file.open(QFile::ReadOnly);
    uint8_t *rawFile = file.map(0, file.size());

    //check magic
    char magic[4];
    memcpy(magic, rawFile, 4);
    if(memcmp(magic,&"libe",4) != 0)
        return -1;

    size_t offset = 4;

    //read time last saved
    uint64_t savedTime;
    memcpy(&savedTime, rawFile + offset, 8);
    offset += 8;
    pSavedTime.fromMSecsSinceEpoch(savedTime);

    //read name
    uint16_t nameLength;
    memcpy(&nameLength, rawFile + offset, 2);
    offset += 2;
    pName.fromLocal8Bit((const char*)rawFile + offset, nameLength);
    offset += nameLength;

    //scrypt vars
    scryptN = *(uint32_t*)(rawFile + offset);
    scryptR = *(uint32_t*)(rawFile + offset + 4);
    scryptP = *(uint32_t*)(rawFile + offset + 8);
    offset += 12;

    //salt & nonce
    memcpy(salt, rawFile + offset, 24);
    offset += 24;
    memcpy(nonce, rawFile + offset, 24);
    offset += 24;

    //block two
    memcpy(&blockTwoLength, rawFile + offset, 8);
    offset += 8;
    blockTwoOffset = offset;

    file.unmap(rawFile);
    file.close();

    return 0;
}

int Profile::saveFile()
{
    /* Create block two */
    size_t blockTwoSize = pDataLength + 36, totalLength;
    uint8_t blockTwoPlaintext[blockTwoSize], blockTwoEncrypted[blockTwoSize];
    uint8_t magic1[4] = {0x72, 0x74, 0x61, 0x73};

    memcpy(blockTwoPlaintext + 32, magic1, 4);
    memcpy(blockTwoPlaintext + 32 + 4, pData, pDataLength);

    //required zerobytes
    memset(blockTwoPlaintext,0,32);

    /* Encrypt block two */
    if(crypto_secretbox(blockTwoEncrypted, blockTwoPlaintext, blockTwoSize, nonce, encryptedKey) != 0)
        return -1;
    memset(blockTwoPlaintext, 0, blockTwoSize);

    /* Compose entire file */
    //determine file size & create buffer
    uint16_t nameLength = pName.length();
    totalLength = blockTwoSize + nameLength + 82;
    uint8_t buffer[totalLength];

    //magic
    uint8_t magic2[4] = {0x6c, 0x69, 0x62, 0x65};
    memcpy(buffer,magic2,4);
    size_t offset = 4;

    //time
    uint64_t saved = time(NULL);
    memcpy(buffer + offset, &saved, 8);
    offset += 8;
    pSavedTime.fromMSecsSinceEpoch(saved);

    //profile name
    memcpy(buffer + offset, &nameLength, 2);
    offset += 2;
    memcpy(buffer + offset, pName.toLocal8Bit().constData(), nameLength);
    offset += nameLength;

    //scrypt values
    memcpy(buffer + offset, &scryptN, 4);
    offset += 4;
    memcpy(buffer + offset, &scryptR, 4);
    offset += 4;
    memcpy(buffer + offset, &scryptP, 4);
    offset += 4;

    //salt & nonce
    memcpy(buffer + offset, salt, 24);
    offset += 24;
    memcpy(buffer + offset, nonce, 24);
    offset += 24;

    //block two
    memcpy(buffer + offset, &blockTwoSize, 8);
    offset += 8;
    memcpy(buffer + offset, &blockTwoEncrypted, blockTwoSize);
    offset += blockTwoSize;

    memset(blockTwoPlaintext,0,blockTwoSize);

    //resize & write file
    QFile file(pPath);
    file.open(QFile::WriteOnly);
    file.resize(totalLength);
    file.write((const char*)buffer,totalLength);
    file.close();

    return 0;
}
