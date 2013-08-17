#ifndef MESSAGEDISPLAYWIDGET2_H
#define MESSAGEDISPLAYWIDGET2_H

#include <QScrollArea>

class QHBoxLayout;
class QVBoxLayout;

/*! New message display widget.
 *
 * CSS classes for styling
 * -----------------------
 *
 * Object         | Qt class   | CSS class
 * ---------------| -----------|--------------
 * Timestamp      | QLabel     | msgTimestamp
 * Username       | ElideLabel | msgUserName
 * Friendname     | ElideLabel | msgFriendName
 * Message        | QLabel     | msgMessage
 * Seperator line | QFrame     | msgLine
 */
class MessageDisplayWidget2 : public QScrollArea
{
    Q_OBJECT
public:
    explicit MessageDisplayWidget2(QWidget *parent = 0);

    void appendMessage(const QString &name, const QString &message);
    void appendErrorMessage(const QString &message);
    
signals:
    
public slots:

private slots:
    void moveScrollBarToBottom(int min, int max);
    
private:
    QVBoxLayout *mainlayout;

    QString lastName;

    QString urlify(const QString &string);
};

#endif // MESSAGEDISPLAYWIDGET2_H
