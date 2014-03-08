#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <QObject>
#include <QHash>
#include <QString>
#include "status.hpp"

enum Sound
{
    Error,
    FriendLogsIn,
    FriendLogsOut,
    FriendRequestAccepted,
    IncommingCall,
    IncommingVideoCall,
    LogIn,
    LogOut,
    NewMessage,
    OutgoingCall,
    OutgoingVideoCall,
    FiletransferComplete,
    FiletransferPending
};
Q_DECLARE_METATYPE(Sound)

class SoundPack
{
public:
    typedef QHash<Sound, QString> SoundList;

    SoundPack();

    bool parse(const QString &themeDirName);

    inline QString themeDirName() { return mThemeDirName; }
    inline QString name() { return mName; }
    inline QString author() { return mAuthor; }
    inline QString description() { return mDescription; }
    inline QString version() { return mVersion; }
    inline QString website() { return mWebsite; }

    inline SoundList list() { return mList; }
    inline bool isValid() { return mValid; }

private:
    QString mThemeDirName;
    QString mName;
    QString mAuthor;
    QString mDescription;
    QString mVersion;
    QString mWebsite;

    SoundList mList;
    bool mValid;
};

class SoundManager : public QObject
{
    Q_OBJECT
public:
    static SoundManager& getInstance();

    static QString directory();

public slots:
    void setSoundPack(const QString &packDirName);
    void playSingleSound(const Sound &sound);

private:
    SoundManager();
    SoundManager(SoundManager &) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    SoundPack mPack;
};

#endif // SOUNDMANAGER_HPP
