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
#include <QDebug>

#include <hunspell/hunspell.hxx>

Spellchecker::Spellchecker(QTextEdit* parent)
    : QSyntaxHighlighter(parent),
      textEdit(parent),
      regEx("(?![-\'])[\\pP|\\pZ|\\pC]"), /* any punctuation character except ' and -, any space character and any "other" character */
      format(),
      skipPosition(NO_SKIPPING),
      cursorChangedByEditing(false)
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

QStringList Spellchecker::suggest(const QString& word)
{
    QStringList suggestions;

    char** slst;
    const int numberOfSuggestions = hunspell->suggest(&slst, word.toLocal8Bit().constData());
    for (int i = 0; i < numberOfSuggestions; i++) {
        suggestions << slst[i];
    }
    hunspell->free_list(&slst, numberOfSuggestions);

    return suggestions;
}

bool Spellchecker::skipRange(int start, int end)
{
    return start <= skipPosition && end >= skipPosition;
}

// returns index number of the token in the block pointed by the given position or -1 if no such token found
int Spellchecker::tokenIndexInBlock(int position, const QTextBlock& block)
{
    int counter = 0;
    const QString text = block.text();
    const int mappedPosition = position - block.position();
    const QStringList tokens = text.split(regEx);

    QStringListIterator it(tokens);
    int start = 0;

    while (it.hasNext() && start <= mappedPosition) {
        start += it.next().length() + 1; // skip the non-word character
        counter++;
    }

    return start <= mappedPosition ? -1 : counter;
}

// sets some variables for Spellchecker::cursorPositionChanged and Spellchecker::highlightBlock, which are called right after this function
void Spellchecker::contentsChanged(int /*position*/, int /*charsRemoved*/, int /*charsAdded*/)
{
    cursorChangedByEditing = true;
    skipPosition = textEdit->textCursor().position();
}

void Spellchecker::cursorPositionChanged()
{
    // when cursor position is changed by editing (i.e. Spellchecker::contentsChanged), the parent class rehighlights everything automatically.
    // but if the cursor position is changed not by editing the text but by simply moving the cursor, we should check and possibly highlight
    // the token our cursor was at before the cursor position changed, since we don't check tokens that we currently have the cursor in.
    // we also need to update currentCursorPosition in case it was changed not by editing.
    if (!cursorChangedByEditing && skipPosition != NO_SKIPPING) {

        const int previousPosition = skipPosition;
        const int currentPosition = textEdit->textCursor().position();

        const QTextBlock previousBlock = textEdit->document()->findBlock(previousPosition);
        const QTextBlock currentBlock  = textEdit->document()->findBlock(currentPosition);

        const int previousTokenIndex = tokenIndexInBlock(previousPosition, previousBlock);
        const int currentTokenIndex  = tokenIndexInBlock(currentPosition,  currentBlock);

        // this should not happen
        if (previousTokenIndex == -1 || currentTokenIndex == -1) {
            qWarning("Wrong token positions (\"%s\" in \"%s:%d\")", Q_FUNC_INFO, __FILE__, __LINE__);
        }

        // if we moved outside token's boundaries -- rehighlight text in the previous position
        if ((previousTokenIndex != currentTokenIndex) || (previousBlock != currentBlock)) {
            skipPosition = NO_SKIPPING;
            rehighlightBlock(textEdit->document()->findBlock(previousPosition));
        } else {
            skipPosition = currentPosition;
        }
    }
    // assume next cursor change won't be done by editing.
    // if it will be actually done by editing, then contentsChanged would set this to true anyway
    cursorChangedByEditing = false;
}
