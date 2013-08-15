#ifndef MESSAGESTYLEINTERFACE_H
#define MESSAGESTYLEINTERFACE_H

#include <QTextBrowser>

class QTextTableCell;

class MessageStyleInterface : public QObject
{
    Q_OBJECT
public:
    void setWidget(QTextBrowser *widget){ mWidget = widget; initStyle();}
    virtual void addMessage(const QString &name, const QString &message) = 0;
    virtual void addErrorMessage(const QString &message) = 0;

protected:
    virtual void initStyle() = 0;

    void urlify(QString& string);
    QTextBrowser *mWidget;
};

class MessageDefaultStyle : public MessageStyleInterface
{
    Q_OBJECT
public:
    void addMessage(const QString &name, const QString &message);
    void addErrorMessage(const QString &message);

private:
    void initStyle();
};

class MessageIrcStyle : public MessageStyleInterface
{
    Q_OBJECT
public:
    void addMessage(const QString &xName, const QString &message);
    void addErrorMessage(const QString &message);


private:
    void initStyle();
    void insertText(QTextTableCell *cell, const QString &text, Qt::Alignment alignment = Qt::AlignLeft, QColor color = Qt::black);

    QTextTable *table;
};

#endif // MESSAGESTYLEINTERFACE_H
