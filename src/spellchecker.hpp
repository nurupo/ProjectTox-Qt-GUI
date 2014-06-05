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

#ifndef SPELLCHECKER_HPP
#define SPELLCHECKER_HPP

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class Hunspell;
class QTextCharFormat;
class QTextEdit;

class Spellchecker : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Spellchecker(QTextEdit*);
    ~Spellchecker();

    bool isCorrect(const QString&);
    QStringList suggest(const QString&);
    bool skipRange(int /*inclusive*/, int /*inclusive*/);

protected:
    void highlightBlock(const QString& text);

private:
    static const int NO_SKIPPING = -1;

    /* the view to highlight */
    QTextEdit* textEdit;

    /* the current used dictionary */
    Hunspell* hunspell;

    /* the regular expression to use for tokenizing each line */
    const QRegularExpression regEx;

    /* the format to apply to misspelled words */
    QTextCharFormat format;

    /* the spell checker will ignore the word which
     * has at least one character at this position (absolute) */
    int skipPosition;

    /* indicates whether the content was changed or not
     * do not use until you know what you are doing */
    bool cursorChangedByEditing;

    /* maps the given absolute position of a character in the document
     * to the reltive index number of the token it belongs to inside its block */
    int tokenIndexInBlock(int, const QTextBlock &block);

private slots:
    void contentsChanged(int, int, int);
    void cursorPositionChanged();

};

#endif // SPELLCHECKER_HPP
