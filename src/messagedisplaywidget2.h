#ifndef MESSAGEDISPLAYWIDGET2_H
#define MESSAGEDISPLAYWIDGET2_H

#include <QScrollArea>

#include "emoticonmenu.h"

class QHBoxLayout;
class QVBoxLayout;

/*! New message display widget.
 *
 * CSS classes for styling
 * -----------------------
 *
 * Object         | Qt class           | CSS class
 * ---------------| -------------------|--------------
 * Timestamp      | QLabel             | msgTimestamp
 * Username       | ElideLabel         | msgUserName
 * Friendname     | ElideLabel         | msgFriendName
 * Message        | QLabel             | msgMessage
 * Seperator line | QFrame             | msgLine
 * Error message  | ElideLabel, QLabel | msgError
 */
class MessageDisplayWidget2 : public QScrollArea
{
    Q_OBJECT
public:
    explicit MessageDisplayWidget2(QWidget *parent = 0);

    void setSmileyList(const EmoticonMenu::SmileyHash &list);

    void appendMessage(const QString &name, const QString &message);
    void appendErrorMessage(const QString &message);

signals:
    
public slots:

private slots:
    void moveScrollBarToBottom(int min, int max);
    
private:
    QVBoxLayout *mainlayout;
    EmoticonMenu::SmileyHash smileyList;
    QString lastName;


    QString urlify(QString string);
    QString smile(QString text);
};

#endif // MESSAGEDISPLAYWIDGET2_H
