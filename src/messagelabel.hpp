#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QLabel>

class MessageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MessageLabel(QWidget *parent = 0);

    void setMessageId(int id);
    int messageId();

private slots:
    void copyPlainText();

private:
    int mId;
};

#endif // MESSAGELABEL_H
