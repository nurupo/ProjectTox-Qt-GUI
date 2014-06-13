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
class QAction;
class QPlainTextEdit;
class QTextCharFormat;
class QTextEdit;

class Spellchecker : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Spellchecker(QTextEdit*);
    Spellchecker(QPlainTextEdit*);
    ~Spellchecker();

    bool isCorrect(const QString&) const;
    bool isCorrect(const QStringRef&) const;
    QStringList suggest(const QString&) const;
    bool skipRange(int /*inclusive*/, int /*inclusive*/) const;

    /** Looks up a word under the cursor and returns its starting and ending positions in
     *  the underlying QTextDocument.
     *  It uses Spellchecker's definition of a word rather than Qt's. Qt interprets
     *  "it's" as three different words: "it", "'" and "s", which is bad for checkspelling.
     *  Currently this function treats words with ' and - in them as a single word, this might
     *  change though if the regEx regular expression will get changed.
     *  If the word under the cursor is not what Spellchecker considers to be a word, it returns
     *  -1 for either startingPosition, endingPosition or both.
     */
    void getWordBoundaries(const QTextCursor& cursor, int* startingPosition, int* endingPosition) const;

    /** A convinient method that looks up what word under the cursor is, checks if it's inrrect, and:
     *  if the word is incorrect, it returns a QList of pointers to QActions that have text of suggested
     *  corrections. Note that QList might be empty if there were no suggestions for the word found.
     *  if the word is correct, it returns an empty QList.
     *
     *  It uses Spellchecker's definition of a word.
     *
     *  The returned QActions should be deleted by the caller (see qDeleteAll()) after actions are used.
     *
     *  When one of QActions is triggered, it will replace the word under the cursor with the correction that is QAction's text.
     *
     *  This function assumes that the document that the cursor belongs to will not be changed between the call to this function and triggering of QActions.
     */
    QList<QAction*> getContextMenuActions(QTextCursor cursor) const;

protected:
    void highlightBlock(const QString& text);

private:
    void init(QTextDocument* document);

    int getWordStartingPosition(QTextCursor cursor, int maxLookup) const;
    int getWordEndingPosition(QTextCursor cursor, int maxLookup) const;

    static const int NO_SKIPPING = -1;

    QTextEdit* textEdit;
    QPlainTextEdit* plainTextEdit;

    /* the document to highlight */
    QTextDocument* document;

    /* the current used dictionary */
    Hunspell* hunspell;

    /* the regular expression to use for tokenizing each line */
    static const QRegularExpression wordDelimiterRegEx;

    static const QRegularExpression exceptionWordDelimiterRegEx;

    /* the format to apply to misspelled words */
    static QTextCharFormat format;

    /* the spell checker will ignore the word which
     * has at least one character at this position (absolute) */
    int skipPosition;

    /* indicates whether the content was changed or not
     * do not use until you know what you are doing */
    bool cursorChangedByEditing;

    /* maps the given absolute position of a character in the document
     * to the reltive index number of the token it belongs to inside its block */
    int tokenIndexInBlock(int, const QTextBlock &block) const;

private slots:
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void cursorPositionChanged();

};

#endif // SPELLCHECKER_HPP
