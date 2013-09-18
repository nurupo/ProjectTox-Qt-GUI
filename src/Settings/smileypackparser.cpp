#include "smileypackparser.h"
#include <QFile>
#include <QDebug>
#include <QRegularExpression>

SmileypackParser::SmileypackParser(const QString &filePath, QObject *parent) :
    QObject(parent)
{
    // Open file
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file.";
        return;
    }

    currentState = StateHead;
    while (!file.atEnd()) {
        QString line(file.readLine());
        if line == "[smileys]"
    }

    line.split()
}
