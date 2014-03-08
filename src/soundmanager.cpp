#include "soundmanager.hpp"
#include "appinfo.hpp"
#include "Settings/settings.hpp"

#include <QSound>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

SoundPack::SoundPack() :
    mValid(false)
{
}

bool SoundPack::parse(const QString &themeDirName)
{
    QDir dir(SoundManager::directory() + '/' + themeDirName);
    if(!dir.exists()) {
        mValid = false;
        return false;
    }

    mThemeDirName = themeDirName;

    QSettings packParser(SoundManager::directory() + '/' + themeDirName + "/theme", QSettings::IniFormat);

    mName        = packParser.value("header/name").toString();
    mAuthor      = packParser.value("header/author").toString();
    mDescription = packParser.value("header/description").toString();
    mVersion     = packParser.value("header/version").toString();
    mWebsite     = packParser.value("header/website").toString();

    if(mName.isEmpty()) {
        mValid = false;
        return false;
    }

    mList.insert(Sound::Error,                packParser.value("sounds/error").toString());
    mList.insert(Sound::FriendLogsIn,         packParser.value("sounds/friendLogsIn").toString());
    mList.insert(Sound::FriendLogsOut,        packParser.value("sounds/friendLogsOut").toString());
    mList.insert(Sound::FriendRequestAccepted,packParser.value("sounds/friendRequestAccepted").toString());
    mList.insert(Sound::IncommingCall,        packParser.value("sounds/incommingCall").toString());
    mList.insert(Sound::IncommingVideoCall,   packParser.value("sounds/incommingVideoCall").toString());
    mList.insert(Sound::LogIn,                packParser.value("sounds/logIn").toString());
    mList.insert(Sound::LogOut,               packParser.value("sounds/logOut").toString());
    mList.insert(Sound::NewMessage,           packParser.value("sounds/newMessage").toString());
    mList.insert(Sound::OutgoingCall,         packParser.value("sounds/outgoingCall").toString());
    mList.insert(Sound::OutgoingVideoCall,    packParser.value("sounds/outgoingVideoCall").toString());
    mList.insert(Sound::FiletransferComplete, packParser.value("sounds/filetransferComplete").toString());
    mList.insert(Sound::FiletransferPending,  packParser.value("sounds/filetransferPending").toString());

    mValid = true;
    return true;
}

// ==============================================

SoundManager &SoundManager::getInstance()
{
    static SoundManager soundManager;
    return soundManager;
}

QString SoundManager::directory()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + AppInfo::name + "/sounds";
}

SoundManager::SoundManager() :
    QObject(nullptr)
{
    setSoundPack(Settings::getInstance().getsoundPackDirName());
}

void SoundManager::setSoundPack(const QString &packDirName)
{
    mPack.parse(packDirName);
}

void SoundManager::playSingleSound(const Sound &sound)
{
    if (!Settings::getInstance().isSoundEnabled() || !Settings::getInstance().isPlayOn(sound) || !mPack.isValid())
        return;

    QString soundFile = mPack.list().value(sound);
    if(soundFile.isEmpty())
        return;

    QSound::play(directory() + '/' + mPack.themeDirName() + '/' + soundFile);
}
