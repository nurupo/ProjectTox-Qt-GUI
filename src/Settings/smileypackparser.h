#ifndef SMILEYPACKPARSER_H
#define SMILEYPACKPARSER_H

#include <QObject>

class SmileypackParser : public QObject
{
    Q_OBJECT
public:
    explicit SmileypackParser(const QString &filePath, QObject *parent = 0);
    
signals:
    
public slots:
    
private:
    enum State{
        StateHead,
        StateSmileys
    };

    State currentState;
};

#endif // SMILEYPACKPARSER_H
