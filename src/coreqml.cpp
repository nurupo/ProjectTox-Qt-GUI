#include <QVariant>

#include "coreqml.hpp"

UserItem::UserItem(int friendId, QString userName, QString userId, QObject *parent) :
    QObject(parent)
{
    m_friendId = friendId;
    m_username = userName;
    m_userid = userId;
}

CoreQml::CoreQml(QObject *parent) :
    QObject(parent)
{
    core = new Core();
    coreThread = new QThread(this);
    core->moveToThread(coreThread);
    connect(coreThread,&QThread::started, core, &Core::start);

    //connect(core, &Core::connected, this, &CoreQml::m_connected);
    //connect(core, &Core::disconnected, this, &CoreQml::m_disconnected);
    connect(core, &Core::userIdGenerated, this, &CoreQml::m_useridgenerated);
    connect(core, &Core::friendAdded, this, &CoreQml::m_friendadded);
    connect(core, &Core::friendRemoved, this, &CoreQml::m_friendremoved);
}

void CoreQml::m_connected(){

}

void CoreQml::m_disconnected(){

}

void CoreQml::m_useridgenerated(const QString &userId){

}

void CoreQml::m_friendadded(int friendId, const QString &userId){
    UserItem *user = new UserItem(friendId, userId);
    m_friendlist.append(user);
    emit friendsChanged();
}

void CoreQml::m_friendremoved(int friendId){
    foreach(QObject *user, m_friendlist){
        if (user->property("friendid").toInt() == friendId)
        {
            m_friendlist.removeOne(user);
            break;
        }
    }

    emit friendsChanged();
}

void CoreQml::acceptFriendRequest(QObject *friendobj){
    QString id = friendobj->property("userid").toString();
    core->acceptFirendRequest(id);
}
