#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QLabel>

#include "emoticonmenu.h"

class MessageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MessageLabel(QWidget *parent = 0);

    void setSmileyList(const EmoticonMenu::SmileyHash &list);
    QString getPlainText();
    
signals:
    
public slots:
    
private slots:
    void copyPlainText();

private:
    EmoticonMenu::SmileyHash smileyHash;
};

#endif // MESSAGELABEL_H
