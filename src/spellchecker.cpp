/*
    Copyright (C) 2013 by retuxx <github@retux.de>

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

#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QTextCharFormat>
#include <QTextEdit>

#include <hunspell/hunspell.hxx>

Spellchecker::Spellchecker(QTextEdit* parent)
    : QSyntaxHighlighter(parent),
      textEdit(parent),
      regEx("\\W"),                 /* any non-word character. */
      format(),
      skippedPosition(NO_SKIPPING), /* skipping should be disabled by default. */
      contentChanged(false)
{
    QString basePath;
#ifdef Q_OS_WIN
    basePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + "hunspell" + '/';
#else
    basePath = "/usr/share/hunspell/";
#endif
    QFileInfo aff(basePath + "en_US.aff");
    QFileInfo dic(basePath + "en_US.dic");
    hunspell = new Hunspell(
        aff.absoluteFilePath().toLocal8Bit().constData(),
        dic.absoluteFilePath().toLocal8Bit().constData()
    );

    format.setUnderlineColor(QColor(255,0,0));
    format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &Spellchecker::cursorPositionChanged);
    connect(textEdit->document(), &QTextDocument::contentsChange, this, &Spellchecker::contentsChanged);
    // this is a simple hack to ensure that the connected slot above
    // will be triggered before the highlighting will be applied.
    setDocument(textEdit->document());

}

Spellchecker::~Spellchecker()
{
    delete hunspell;
}

void Spellchecker::highlightBlock(const QString& text)
{
    const QStringList tokens = text.split(regEx);
    QStringListIterator it(tokens);
    const int offset = currentBlock().position();
    int start, length, end;
    start = length = end = 0;

    while (it.hasNext()) {
        const QString& token  = it.next();
        length = token.length();
        end    = start + length;

        if (!skipRange(offset + start, offset + end) &&
            !isCorrect(token)) {
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
    const int numberOfSuggestions = hunspell->suggest(&slst, word.toLocal8Bit().constData());
    for (int i = 0; i < numberOfSuggestions; i++) {
        suggestions << slst[i];
    }
}

bool Spellchecker::skipRange(int start, int end)
{
    return skippedPosition >= start && skippedPosition <= end;
}

int Spellchecker::toPositionInBlock(int position)
{
    int counter = 0;
    const QTextBlock block = textEdit->document()->findBlock(position);
    const QString text = block.text();
    const int mappedPosition = position - block.position();
    const QStringList tokens = text.split(regEx);

    QStringListIterator it(tokens);
    int start = 0;

    while (it.hasNext() && start < mappedPosition) {
        start += it.next().length() + 1; // skip the non-word character
        counter++;
    }

    return start < mappedPosition ? -1 : counter;
}

void Spellchecker::contentsChanged(int position, int charsRemoved, int charsAdded)
{
    contentChanged = true;
    skippedPosition = textEdit->textCursor().position();
}

void Spellchecker::cursorPositionChanged()
{
    // block signal if content was changed because
    // skipping was already handled by slot Spellchecker::contentsChanged();
    if (!contentChanged && skippedPosition >= 0) {
        const int pos     = textEdit->textCursor().position();
        const int current = toPositionInBlock(skippedPosition);
        const int moved   = toPositionInBlock(pos);

        if (current >= 0 && moved >= 0 && current == moved) {
            skippedPosition = pos;
        } else {
            skippedPosition = NO_SKIPPING;
        }

        rehighlight();
    }

    contentChanged = false;
}
