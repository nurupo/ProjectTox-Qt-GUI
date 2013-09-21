#ifndef SMILEYPACKPARSER_H
#define SMILEYPACKPARSER_H

#include <QObject>
#include <QHash>
#include <QStringList>




class SmileypackParser : public QObject
{
    Q_OBJECT
public:
    typedef QHash<QString, QString> HeaderHash;
    typedef QHash<QString,QStringList> SmileyHash;

    explicit SmileypackParser(QObject *parent = 0);

    bool parseFile(const QString &filePath);
    HeaderHash getHeader()  { return headHash;   }

    static SmileyHash &getSmileyHash();
    
signals:
    
public slots:
    
private:
    // States for parser "state machine"
    enum States{
        StateHead,
        StateSmileys
    };

    States  state;
    QString errorMessage;

    HeaderHash headHash;
    static SmileyHash smileyHash;

    void processLine(const QString &xLine, const QString &xPath);


};

Q_DECLARE_METATYPE(SmileypackParser::HeaderHash)
Q_DECLARE_METATYPE(SmileypackParser::SmileyHash)

#endif // SMILEYPACKPARSER_H
