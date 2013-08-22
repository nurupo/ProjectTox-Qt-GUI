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
 * Object         | Qt class           | CSS class
 * ---------------| -------------------|--------------
 * Timestamp      | QLabel             | msgTimestamp
 * Username       | ElideLabel         | msgUserName
 * Friendname     | ElideLabel         | msgFriendName
 * Message        | QLabel             | msgMessage
 * Seperator line | QFrame             | msgLine
 * Error message  | ElideLabel, QLabel | msgError
 */
class MessageDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit MessageDisplayWidget(QWidget *parent = 0);

    void appendMessage(const QString &name, const QString &message, int messageId = -1);

signals:
    
public slots:

private slots:
    void moveScrollBarToBottom(int min, int max);
    
private:
    QVBoxLayout *mainlayout;
    QString lastName;

    QString urlify(QString string);
};

#endif // MESSAGEDISPLAYWIDGET2_H
