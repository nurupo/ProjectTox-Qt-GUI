#include "smileypackparser.h"
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>

SmileypackParser::SmileypackParser(QObject *parent) :
    QObject(parent)
{
}

bool SmileypackParser::parseFile(const QString &filePath)
{
    // Open file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = tr("Unable to open smileypack theme file.");
        return false;
    }

    // Read lines
    state = StateHead;
    while (!file.atEnd()) {
        processLine(file.readLine(), filePath);
    }

    //qDebug() << headHash;
    //qDebug() << smileyHash;
    //qDebug() << "-----------------------";

    file.close();
    return true;
}

SmileypackParser::SmileyHash &SmileypackParser::getSmileyHash()
{
    static SmileypackParser::SmileyHash smileyHash;
    return smileyHash;
}

void SmileypackParser::processLine(const QString &xLine, const QString &xPath)
{
    // Trim spaces and exclue comment lines
    QString line = xLine.trimmed();
    if(line.startsWith('#') || line.isEmpty()) {
        return;
    }
    
    if (state == StateHead) {
        QString key   = line.section(QRegularExpression("\\s*\\=\\s*"), 0,0);
        QString value = line.section(QRegularExpression("\\s*\\=\\s*"), 1);

        headHash.insert(key, value);

        // Switch on [xxx]
        QRegularExpression rx("\\[(theme|smileys)\\]");
        QRegularExpressionMatch match = rx.match(line);
        if (match.hasMatch()) {
            state = StateSmileys;
            return;
        }
    }
    else if (state == StateSmileys) {
        QString key   = line.section(QRegularExpression("\\s+"), 0,0);
        QString value = line.section(QRegularExpression("\\s+"), 1);

        QFileInfo f(xPath);
        key.prepend(f.absolutePath() + QDir::separator());

        getSmileyHash().insert(key, value.split(QRegularExpression("\\s+")));
    }
}
