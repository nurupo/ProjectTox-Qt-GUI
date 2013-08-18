#ifndef EMOTICONMENU_H
#define EMOTICONMENU_H

#include <QMenu>

class QGridLayout;

class EmoticonMenu : public QMenu
{
    Q_OBJECT
public:
    typedef QHash<QString,QStringList> SmileyList;

    explicit EmoticonMenu(QWidget *parent = 0);

    SmileyList getSmileyList();
    
signals:
    void insertEmoticon(QString);
    
public slots:
    
private slots:
    void onEmoticonTriggered();

private:
    void addEmoticon(const QString &imgPath, const QStringList &texts);

    QGridLayout *layout;
    SmileyList smileyList;
};

#endif // EMOTICONMENU_H
