#ifndef SMILEYPACK_H
#define SMILEYPACK_H

#include <QObject>
#include <QPair>
#include <QStringList>


class Smileypack : public QObject
{
    Q_OBJECT
public:
    typedef QList<QPair<QString, QStringList>> SmileyList;

    explicit Smileypack(QObject *parent = 0);
    void operator=(const Smileypack &other);

    bool parseFile(const QString &filePath);

    const QString &getName() const { return name; }
    const QString &getAuthor() const { return author; }
    const QString &getDescription() const { return description; }
    const QString &getVersion() const { return version; }
    const QString &getWebsite() const { return website; }
    const QString &getIcon() const { return icon; }
    const SmileyList &getList() const { return list; }
    void  setList(const SmileyList &xList) { list = xList; }

    bool isEmoij() const { return emoij; }
    void setEmoij();

    // Current seleced Smileypack functions
    static const QString& packDir();
    static Smileypack &currentPack();
    static QString smile(QString text);
    static QString desmile(QString htmlText);

private:
    QString name;
    QString author;
    QString description;
    QString version;
    QString website;
    QString icon;
    SmileyList list;
    bool emoij;

    static const SmileyList emoijList;

    // Parser functions
    enum ParserStates{
        StateHead,
        StateSmileys
    };
    void processLine(const QString &xLine, const QString &xPath, ParserStates &xState);
};

#endif // SMILEYPACK_H
