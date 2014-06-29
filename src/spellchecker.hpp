/*
    Copyright (C) 2014 by retuxx <github@retux.de>
                  2014 by Maxim Biro <nurupo.contributions@gmail.com>

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
class QActionGroup;
class QPlainTextEdit;
class QTextCharFormat;
class QTextEdit;

/**
 * Class for spell checking in QTextEdit or QPlainText.
 *
 * It uses a different definition of a word than Qt does. Qt interprets "it's"
 * as three different words: "it", "'" and "s", which is bad for checkspelling.
 * Instead we treat ''' (apostrophe) and '-' (hyphen) as part of a word, as
 * long as they appear between word characters. This makes text highlighting
 * and lookup of the word under the cursor a lot more expensive than using Qt's
 * definition of a word, but it's worth it.
 */

class Spellchecker : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Spellchecker(QTextEdit*);
    Spellchecker(QPlainTextEdit*);
    ~Spellchecker();

    static bool isCorrect(const QString& word);
    static bool isCorrect(const QStringRef& word);
    static QStringList suggest(const QString& word);
    bool skipRange(int start, int end) const;

    /**
     * Looks up a word under the cursor and returns its starting and ending positions in
     * If the word under the cursor is not what Spellchecker considers to be a word, it returns
     * -1 for either startingPosition, endingPosition or both.
     */
    static void getWordBoundaries(const QTextCursor& cursor, int* startingPosition, int* endingPosition);

    static bool isEnabled() {return enabled;}

    static bool hasDictionarySet() {return hunspell != nullptr;}

    static QStringList getAvailableLanguages() {return dictionaries.keys();}

    static QString getLanguage() {return getLanguageName(dictionaryName);}

    static QString getDictionaryDirPath() {return dictionaryDirPath;}
    static QString getDictionaryName() {return dictionaryName;}

public slots:
    static void setEnabled(bool enabled) {Spellchecker::enabled = enabled; rehighlightAll();}

    /**
     * Set spell check language from the getAvailableLanguages() list.
     * Don't use for saving/loading since it might differ from enviroment to enviroment, use DictionaryName
     * instead.
     */
    static void setLanguage(const QString& language) {setDictionaryName(dictionaries[language]);}

    static void setDictionaryDirPath(const QString& dictDirPath);
    static void setDictionaryName(const QString& dictName);

protected:
    void highlightBlock(const QString& text) Q_DECL_OVERRIDE;

    /* the regular expression to use for tokenizing each line */
    static const QRegularExpression wordDelimiterRegEx;

    static const QRegularExpression exceptionWordDelimiterRegEx;

private:
    /* the current used dictionary */
    static Hunspell* hunspell;

    static QString getLanguageName(const QString& dictionaryName);

    static bool enabled;

    static QString dictionaryDirPath;
    static QString dictionaryName;
    /* <human-readable language name, dictionary file name> */
    static QMap<QString, QString> dictionaries;

    static Hunspell* getHunspellForDictionary(const QString& dictionaryName);

    static QList<Spellchecker*> instances;
    static void rehighlightAll();

    void init(QTextDocument* document);

    static int getWordStartingPosition(QTextCursor cursor, int maxLookup);
    static int getWordEndingPosition(QTextCursor cursor, int maxLookup);

    static const int NO_SKIPPING = -1;

    QTextEdit* textEdit;
    QPlainTextEdit* plainTextEdit;

    /* the document to highlight */
    QTextDocument* document;

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
    static int tokenIndexInBlock(int position, const QTextBlock& block);

private slots:
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void cursorPositionChanged();
    static void updateDictionaries();
};

/**
 * A convenient class that handles everything you need for a simple spellcheck
 * context menu.
 */

class ContextMenuEnabledSpellchecker : public Spellchecker
{
    Q_OBJECT
public:
    ContextMenuEnabledSpellchecker(QTextEdit* editor) : Spellchecker(editor) {}
    ContextMenuEnabledSpellchecker(QPlainTextEdit* editor) : Spellchecker(editor) {}

    /**
     * Looks up what word under the cursor is, checks if it's incorrect, and if the word is incorrect,
     * it returns a QList of pointers to QActions that have text of suggested corrections, otherwise,
     * if the word under the cursor is correct, it returns an empty QList. Note that QList might be
     * empty if there were no suggestions found for an incorrect word.
     *
     * For cursor pass QTextEdit or QPlainTextEdit ::cursorForPosition(contextMenuPosition)
     *
     * The returned QActions should be deleted by the caller (see qDeleteAll()) after actions are used.
     *
     * When one of QActions is triggered, it will replace the word under the cursor with the correction,
     * which is QAction's text.
     *
     * This function assumes that the document that the cursor belongs to will not be changed between
     * the call to this function and triggering of QActions.
     */
    QList<QAction*> getContextMenuSuggestions(QTextCursor cursor) const;

    /**
     * Returns a QActionGroup that contains actions corresponding for available for spell checking
     * languages or no QActions if there are no available languages. Each QAction has a text set
     * describing the language in UI-friendly way, so there is no need to edit those. When an action
     * is triggered, it changes language used by spellchecker to the selected one.
     *
     * The returned QActionGroup should be deleted by the caller.
     */
    static QActionGroup* getContextMenuLanguages();

    /**
     * Returns a checkable QAction that shows if spell check is enabled and sets the enable state when
     * triggered.
     *
     * The returned QAction shoul be deleted by the caller.
     */
    static QAction* getContextMenuEnableSpellchecking();

};

#endif // SPELLCHECKER_HPP
