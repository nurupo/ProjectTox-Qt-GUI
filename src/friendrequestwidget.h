#ifndef FRIENDREQUESTLABEL_H
#define FRIENDREQUESTLABEL_H

#include <QWidget>

class QLabel;
class ElideLabel;
class QPushButton;
class QToolButton;
class QMenu;

class FriendRequestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FriendRequestWidget(QWidget *parent = 0);
    
signals:
    void userAccepted(QString);
    
public slots:
    void addFriendRequest(const QString &userId, const QString &msg);

private slots:
    void acceptUser();
    void rejectUser();

private:
    QToolButton *button;
    QMenu       *menu;
    QWidget     *actionDefaultWidget;
    ElideLabel  *clientId;
    QLabel      *message;
    QPushButton *accaptButton;
    QPushButton *rejectButton;

    QHash<QString, QString>                requests;
    QMutableHashIterator<QString, QString> it;

    void updateGui();
};

#endif // FRIENDREQUESTLABEL_H
