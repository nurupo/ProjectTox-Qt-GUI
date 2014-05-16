/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>

    This file is part of Tox Qt GUI.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

#include "spellchecker.hpp"

#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QRegExp>
#include <QTextCharFormat>
#include <QFileInfo>

Spellchecker::Spellchecker(QTextDocument* document)
    : QSyntaxHighlighter(document), regEx("\\W") /* any non-word character */
{
    QFileInfo aff("/usr/share/hunspell/en_US.aff");
    QFileInfo dic("/usr/share/hunspell/en_US.dic");
    hunspell = new Hunspell(
        aff.absoluteFilePath().toLocal8Bit().constData(),
        dic.absoluteFilePath().toLocal8Bit().constData()
    );

    format.setUnderlineColor(QColor(255,0,0));
    format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
}

Spellchecker::~Spellchecker()
{
    delete hunspell;
}

void Spellchecker::highlightBlock(const QString& text)
{
    const QStringList tokens = text.split(regEx);
    QStringListIterator it(tokens);
    QString token;
    int start, length;
    start = length = 0;

    while (it.hasNext()) {
        token = it.next();
        length = token.length();

        if (!isCorrect(token)) {
            setFormat(start, length, format);
        }

        start += length + 1; // skip the non-word character
    }
}

bool Spellchecker::isCorrect(const QString& word)
{
    return hunspell->spell(word.toLocal8Bit().constData()) != 0;
}

void Spellchecker::suggest(const QString& word, QStringList& suggestions)
{
    char** slst;
    int n = hunspell->suggest(&slst, word.toLocal8Bit().constData());
    while (n-- > 0) {
        suggestions << slst[n];
    }
}
