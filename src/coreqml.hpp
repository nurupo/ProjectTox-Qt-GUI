#ifndef COREQML_HPP
#define COREQML_HPP

#include <QObject>
#include <QThread>
#include <QList>

#include "core.hpp"

class UserItem : public QObject{
    Q_OBJECT

    Q_PROPERTY(int friendId READ friendId)
    Q_PROPERTY(QString userid READ userid)

    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
public:
    explicit UserItem(int friendId,  QString userId, QString userName = "",QObject *parent = 0);

    int friendId(){return m_friendId;}
    QString userid(){return m_userid;}
    QString username(){ return m_username;}
signals:
    void usernameChanged();
private:
    int m_friendId;
    QString m_username;
    QString m_userid;
};

class CoreQml : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> friends READ friends NOTIFY friendsChanged)
public:
    explicit CoreQml(QObject *parent = 0);

    QList<QObject*> friends(){return m_friendlist;}
private:
    Core *core;
    QThread *coreThread;
    QList<QObject*> m_friendlist;

signals:
    void friendsChanged();

    void connected();
    void disconnected();
    void friendRequest();

public slots:
    void acceptFriendRequest(QObject *friendobj);
private slots:
    void m_connected();
    void m_disconnected();
    void m_useridgenerated(const QString& userId);
    void m_friendadded(int friendId, const QString& userId);
    void m_friendremoved(int friendId);
};

#endif // COREQML_HPP
