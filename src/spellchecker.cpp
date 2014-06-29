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

#include "spellchecker.hpp"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QLocale>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>

#include <hunspell/hunspell.hxx>

// any punctuation character except ' and -, any space character and any "other" character, some of symbol character and Hiragana/Katakana/Kanji
#define WORD_DELIMITER_REGEX "[\\pP\\pZ\\pC\\p{Sm}\\p{Sc}\\p{Sk}\\p{Hiragana}\\p{Katakana}\\p{Han}]"
// except - and '
#define EXCEPTION_WORD_DELIMITER_REGEX "[-\']"

const QRegularExpression Spellchecker::wordDelimiterRegEx = QRegularExpression("(?!" EXCEPTION_WORD_DELIMITER_REGEX ")" WORD_DELIMITER_REGEX);
const QRegularExpression Spellchecker::exceptionWordDelimiterRegEx = QRegularExpression(EXCEPTION_WORD_DELIMITER_REGEX);

QTextCharFormat Spellchecker::format = []() -> QTextCharFormat
{
    QTextCharFormat format;
    format.setUnderlineColor(QColor(Qt::red));
    format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    return format;
}();

bool Spellchecker::enabled = true;
Hunspell* Spellchecker::hunspell = nullptr;
QMap<QString, QString> Spellchecker::dictionaries;
QString Spellchecker::dictionaryDirPath;
QString Spellchecker::dictionaryName;
QList<Spellchecker*> Spellchecker::instances;

void Spellchecker::setDictionaryName(const QString &dictName)
{
    if (hunspell != nullptr) {
        delete hunspell;
    }
    hunspell = getHunspellForDictionary(dictName);
    // only disable. don't enable here, user should manually do so
    if (hunspell == nullptr) {
        enabled = false;
    }
    dictionaryName = dictName;
    rehighlightAll();
}

void Spellchecker::setDictionaryDirPath(const QString &dictDirPath)
{
    static const int dictionaryDirWatcherTimerTimeout = 1000;

    static QTimer* dictionaryDirWatcherTimer = []() -> QTimer*
    {
        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, &Spellchecker::updateDictionaries);
        return timer;
    }();

    static QFileSystemWatcher* dictionaryDirWatcher = [](QTimer* dictionaryDirWatcherTimer, int dictionaryDirWatcherTimerTimeout) -> QFileSystemWatcher*
    {
        QFileSystemWatcher* watcher = new QFileSystemWatcher();
        connect(watcher, &QFileSystemWatcher::directoryChanged, [dictionaryDirWatcherTimer, dictionaryDirWatcherTimerTimeout]() {dictionaryDirWatcherTimer->start(dictionaryDirWatcherTimerTimeout);} );
        return watcher;
    }(dictionaryDirWatcherTimer, dictionaryDirWatcherTimerTimeout);

    const QStringList watchedDirectories = dictionaryDirWatcher->directories();
    if (watchedDirectories.size() != 0) {
        dictionaryDirWatcher->removePaths(watchedDirectories);
    }
    dictionaryDirPath = dictDirPath;
    updateDictionaries();
    dictionaryDirWatcher->addPath(dictionaryDirPath);
}

Hunspell* Spellchecker::getHunspellForDictionary(const QString &dictName)
{
    QString affPath(dictionaryDirPath + dictName + ".aff");
    QString dicPath(dictionaryDirPath + dictName + ".dic");
    qDebug() << Q_FUNC_INFO << dicPath << QFileInfo::exists(affPath) << QFileInfo::exists(dicPath);
    QFileInfo affFileInfo(affPath);
    QFileInfo dicFileInfo(dicPath);
    if (affFileInfo.exists() && affFileInfo.isFile() &&
        dicFileInfo.exists() && dicFileInfo.isFile()) {

        return new Hunspell(
            affPath.toLocal8Bit().constData(),
            dicPath.toLocal8Bit().constData()
        );
    }
    return nullptr;
}

void Spellchecker::updateDictionaries()
{
    QDir hunspellDir(dictionaryDirPath);
    QStringList affList = hunspellDir.entryList(QStringList() << "*.aff", QDir::Files, QDir::Name);
    QStringList dicList = hunspellDir.entryList(QStringList() << "*.dic", QDir::Files, QDir::Name);

    dictionaries.clear();

    for (int i = 0, j = 0, min = qMin(affList.size(), dicList.size()); i < min && j < min; ) {
        const QStringRef affFilename = affList[i].leftRef(affList[i].length() - 4);
        const QStringRef dicFilename = dicList[i].leftRef(dicList[i].length() - 4);
        const int compare = affFilename.compare(dicFilename);
        if (compare < 0) {
            i ++;
        } else if (compare > 0) {
            j ++;
        } else {
            // found matching .aff and .dic
            i ++;
            j ++;
            QString hunspellDictionaryName(affFilename.toString());
            QString humanReadableDictionaryName = getLanguageName(hunspellDictionaryName);

            dictionaries[humanReadableDictionaryName] = hunspellDictionaryName;
            qDebug() << Q_FUNC_INFO << humanReadableDictionaryName << hunspellDictionaryName;
        }
    }

    // if current language dict is removed (not on the list), it would be weird that we are using it
    // so try to reload the dict.
    if (!dictionaryName.isEmpty()) {
        setDictionaryName(dictionaryName);
    }
}

QString Spellchecker::getLanguageName(const QString &dictionaryName)
{
    QLocale locale(dictionaryName);

    QString languageEnumName = locale.languageToString(locale.language());
    QString countryEnumName = locale.countryToString(locale.country());
    auto addSpaces = [](QString& str) {
        for (int i = 0; i < str.length() - 1; i ++) {
            const QCharRef c1 = str[i];
            const QCharRef c2 = str[i+1];
            if (c1.isLetter() && c2.isLetter() && c1.isLower() && c2.isUpper()) {
                str.insert(i+1, " ");
                i ++;
            }
        }
    };
    addSpaces(languageEnumName);
    addSpaces(countryEnumName);

    QString nativeLanguageName(locale.nativeLanguageName());
    QString nativeCountryName(locale.nativeCountryName());
    auto capitalizeFirstLetter = [](QString& str) {
        const QCharRef c = str[0];
        if (c.isLower()) {
            str[0] = c.toUpper();
        }
    };
    capitalizeFirstLetter(nativeLanguageName);
    capitalizeFirstLetter(nativeCountryName);

    return QString("%1 (%2) / %3 (%4)").arg(languageEnumName, countryEnumName,
                                            nativeLanguageName, nativeCountryName);
}

void Spellchecker::rehighlightAll()
{
    for (Spellchecker* i : instances) {
        i->rehighlight();
    }
}

Spellchecker::Spellchecker(QTextEdit* parent)
    : QSyntaxHighlighter(parent),
      skipPosition(NO_SKIPPING),
      cursorChangedByEditing(false)
{
    // we can't use QTextDocument's cursorPositionChanged because it doesn't get invoked when the cursor is changed by mouse interaction
    connect(parent, &QTextEdit::cursorPositionChanged, this, &Spellchecker::cursorPositionChanged);
    textEdit = parent;
    plainTextEdit = nullptr;
    init(parent->document());
}

Spellchecker::Spellchecker(QPlainTextEdit* parent)
    : QSyntaxHighlighter(parent),
      skipPosition(NO_SKIPPING),
      cursorChangedByEditing(false)
{
    // we can't use QTextDocument's cursorPositionChanged because it doesn't get invoked when the cursor is changed by mouse interaction
    connect(parent, &QPlainTextEdit::cursorPositionChanged, this, &Spellchecker::cursorPositionChanged);
    textEdit = nullptr;
    plainTextEdit = parent;
    init(parent->document());
}

void Spellchecker::init(QTextDocument* document)
{
    instances << this;

    this->document = document;

    connect(document, &QTextDocument::contentsChange, this, &Spellchecker::contentsChange);
    // this is a simple hack to ensure that the connected slot above
    // will be triggered before the highlighting will be applied.
    setDocument(document);
}

Spellchecker::~Spellchecker()
{
    instances.removeOne(this);
}

void Spellchecker::highlightBlock(const QString& text)
{
    if (!isEnabled() || !hasDictionarySet()) {
        return;
    }

    const QStringList tokens = text.split(wordDelimiterRegEx);
    QStringListIterator tokensIterator(tokens);

    const int offset = currentBlock().position();

    int start, length;
    start = length = 0;

    while (tokensIterator.hasNext()) {
        const QString& token = tokensIterator.next();

        // split on two consecutive exceptionWordDelimiterRegEx
        QList<QStringRef> subtokens;

        if (token.length() > 0) {
            int previousMatchEnd = -1;
            int previousSplitPos = 0;
            QRegularExpressionMatchIterator exceptionWordDelimiterMatchIterator = exceptionWordDelimiterRegEx.globalMatch(token);
            while (exceptionWordDelimiterMatchIterator.hasNext()) {
                QRegularExpressionMatch match = exceptionWordDelimiterMatchIterator.next();
                if (previousMatchEnd + 1 == match.capturedEnd()) {
                    subtokens << token.midRef(previousSplitPos, match.capturedEnd() - previousSplitPos);
                    previousSplitPos = match.capturedEnd();
                }
                previousMatchEnd = match.capturedEnd();
            }
            if (previousSplitPos != token.length()) {
                subtokens << token.midRef(previousSplitPos);
            }
        } else {
            subtokens << token.midRef(0);
        }

        for (const QStringRef& subtoken : subtokens) {

            length = subtoken.length();

            // trim any exceptionWordDelimiterRegEx
            QStringRef trimmedSubtoken = subtoken;

            // trim front
            int trimmedSubtokenStartOffset = 0;

            for (int i = 0; i < trimmedSubtoken.length(); i ++) {
                if (exceptionWordDelimiterRegEx.match(QString(trimmedSubtoken.at(i))).hasMatch()) {
                    trimmedSubtokenStartOffset ++;
                    length --;
                } else {
                    break;
                }
            }

            trimmedSubtoken = trimmedSubtoken.mid(trimmedSubtokenStartOffset);

            // trim back
            for (int i = trimmedSubtoken.length() - 1; i >= 0; i --) {
                if (exceptionWordDelimiterRegEx.match(QString(trimmedSubtoken.at(i))).hasMatch()) {
                    length --;
                } else {
                    break;
                }
            }

            trimmedSubtoken = trimmedSubtoken.mid(0, length);

            if (trimmedSubtoken.length() != 0 &&
                    !skipRange(offset + start + trimmedSubtokenStartOffset, offset + start + trimmedSubtokenStartOffset + trimmedSubtoken.length()) &&
                    !isCorrect(trimmedSubtoken)) {
                setFormat(start + trimmedSubtokenStartOffset, length, format);
            }

            start += subtoken.length();
        }

        start += 1; // skip the non-word character
    }
}

bool Spellchecker::isCorrect(const QString& word)
{
    if (!hasDictionarySet()) {
        return false;
    }

    return hunspell->spell(word.toLocal8Bit().constData()) != 0;
}

bool Spellchecker::isCorrect(const QStringRef& word)
{
    if (!hasDictionarySet()) {
        return false;
    }

    return hunspell->spell(word.toLocal8Bit().constData()) != 0;
}

QStringList Spellchecker::suggest(const QString& word)
{
    QStringList suggestions;

    if (!hasDictionarySet()) {
        return suggestions;
    }

    char** slst;
    const int numberOfSuggestions = hunspell->suggest(&slst, word.toLocal8Bit().constData());
    for (int i = 0; i < numberOfSuggestions; i++) {
        suggestions << slst[i];
    }
    hunspell->free_list(&slst, numberOfSuggestions);

    return suggestions;
}

bool Spellchecker::skipRange(int start, int end) const
{
    return start <= skipPosition && end >= skipPosition;
}

// returns index number of the token in the block pointed by the given position or -1 if no such token found
int Spellchecker::tokenIndexInBlock(int position, const QTextBlock& block)
{
    int counter = 0;
    const QString text = block.text();
    const int mappedPosition = position - block.position();
    const QStringList tokens = text.split(wordDelimiterRegEx);

    QStringListIterator it(tokens);
    int start = 0;

    while (it.hasNext() && start <= mappedPosition) {
        start += it.next().length() + 1; // skip the non-word character
        counter++;
    }

    return start <= mappedPosition ? -1 : counter;
}

// sets some variables for Spellchecker::cursorPositionChanged and Spellchecker::highlightBlock, which are called right after this function
void Spellchecker::contentsChange(int position, int /*charsRemoved*/, int charsAdded)
{
    if (!isEnabled() || !hasDictionarySet()) {
        return;
    }

    cursorChangedByEditing = true;
    skipPosition = position + charsAdded;
}

void Spellchecker::cursorPositionChanged()
{
    if (!isEnabled() || !hasDictionarySet()) {
        return;
    }

    // when cursor position is changed by editing (i.e. Spellchecker::contentsChanged), the parent class rehighlights everything automatically.
    // but if the cursor position is changed not by editing the text but by simply moving the cursor, we should check and possibly highlight
    // the token our cursor was at before the cursor position changed, since we don't check tokens that we currently have the cursor in.
    // we also need to update currentCursorPosition in case it was changed not by editing.
    if (!cursorChangedByEditing && skipPosition != NO_SKIPPING) {

        const int previousPosition = skipPosition;
        const int currentPosition = plainTextEdit == nullptr ? textEdit->textCursor().position() : plainTextEdit->textCursor().position();

        const QTextBlock previousBlock = document->findBlock(previousPosition);
        const QTextBlock currentBlock  = document->findBlock(currentPosition);

        const int previousTokenIndex = tokenIndexInBlock(previousPosition, previousBlock);
        const int currentTokenIndex  = tokenIndexInBlock(currentPosition,  currentBlock);

        // this should not happen
        if (previousTokenIndex == -1 || currentTokenIndex == -1) {
            qWarning("Wrong token positions (\"%s\" in \"%s:%d\")", Q_FUNC_INFO, __FILE__, __LINE__);
        }

        // if we moved outside token's boundaries -- rehighlight text in the previous position
        if ((previousTokenIndex != currentTokenIndex) || (previousBlock != currentBlock)) {
            skipPosition = NO_SKIPPING;
            rehighlightBlock(document->findBlock(previousPosition));
        } else {
            skipPosition = currentPosition;
        }
    }
    // assume next cursor change won't be done by editing.
    // if it will be actually done by editing, then contentsChanged would set this to true anyway
    cursorChangedByEditing = false;
}

int Spellchecker::getWordStartingPosition(QTextCursor cursor, int maxLookup)
{
    QTextCursor textSelectingCursor(cursor);
    textSelectingCursor.select(QTextCursor::WordUnderCursor);
    QString word = textSelectingCursor.selectedText();

    // after selecting a word, the cursor automatically moves to word's end
    int nextWordStartPosition = textSelectingCursor.position() - word.length();

    QString firstChar = QString(word[0]);

    if (wordDelimiterRegEx.match(firstChar).hasMatch()) {
        // we return -1 because if at least one char matches this, the word we are processing consists fully of puncuation symbols
        // which we don't highlight when checkspelling, so there is no point in looking up spelling of those
        return -1;
    }

    if (word.count(exceptionWordDelimiterRegEx) >= 2) {
        return -1;
    }

    bool firstWord = true;
    bool broke = true;

    cursor.movePosition(QTextCursor::StartOfWord);

    for (int i = 0; i < maxLookup; i ++) {
        // if there is no previous word or we reached the start of block
        // (previous word acts a bit strange, it gives us current word if we are not at the beggining of it, so we check if we at the beginning of the block too)
        if (!cursor.movePosition(QTextCursor::PreviousWord) || (cursor.block().position() == nextWordStartPosition)) {
            break;
        }

        broke = true;

        // a hack to get current word without messing up the cursor
        // TODO: maybe try maing it work with just one cursor?
        textSelectingCursor = QTextCursor(cursor);
        textSelectingCursor.select(QTextCursor::WordUnderCursor);
        word = textSelectingCursor.selectedText();

        int currentWordStartPosition = cursor.position();
        int currentWordEndPosition = currentWordStartPosition + word.length();

        // if there was a non-word character between two words (likely a space character)
        if (nextWordStartPosition - currentWordEndPosition != 0) {
            break;
        }

        firstChar = QString(word[0]);
        if (wordDelimiterRegEx.match(firstChar).hasMatch()) {
            break;
        }

        if (word.count(exceptionWordDelimiterRegEx) >= 2) {
            break;
        }

        firstWord = false;
        broke = false;

        nextWordStartPosition = currentWordStartPosition;
    }

    // if a word starts with one of exceptionWordDelimiterRegEx chars, then remove it from selection
    if (exceptionWordDelimiterRegEx.match(word).capturedEnd() == 1 && !firstWord && !broke) {
        nextWordStartPosition += 1;
    }

    return nextWordStartPosition;
}

int Spellchecker::getWordEndingPosition(QTextCursor cursor, int maxLookup)
{
    QTextCursor textSelectingCursor(cursor);
    textSelectingCursor.select(QTextCursor::WordUnderCursor);
    QString word = textSelectingCursor.selectedText();

    // after selecting a word, the cursor automatically moves to word's end
    int previousWordEndPosition = textSelectingCursor.position();

    int capturedStart = wordDelimiterRegEx.match(word).capturedStart();

    if (capturedStart == 0) {
        // no word
        return -1;
    } else if (capturedStart != -1) {
        // there are some symbols that we consider as word delimeters at the end of the word, so
        // just subtract those to get word end position and return
        previousWordEndPosition -= word.length() - capturedStart;

        return previousWordEndPosition;
    }

    if (word.count(exceptionWordDelimiterRegEx) >= 2) {
        return -1;
    }

    bool firstWord = true;
    bool broke = true;

    for (int i = 0; i < maxLookup; i ++) {
        // if there is no next word or we reached the end of block
        // (next word acts a bit strange, it gives us current word if we are not in the end of it, so we check if we at the end of the block too)
        // (previousWordEndPosition - cursor.block().position()) gives us in-block position
        if (!cursor.movePosition(QTextCursor::NextWord) || (previousWordEndPosition - cursor.block().position()) - (cursor.block().length() - 1) == 0) {
            break;
        }

        broke = true;

        // a hack to get current word without messing up the cursor
        // TODO: maybe try maing it work with just one cursor?
        textSelectingCursor = QTextCursor(cursor);
        textSelectingCursor.select(QTextCursor::WordUnderCursor);
        word = textSelectingCursor.selectedText();

        int currentWordStartPosition = cursor.position();

        // if there was a non-word character between two words (likely a space character)
        if (previousWordEndPosition - currentWordStartPosition != 0) {
            break;
        }

        if (word.count(exceptionWordDelimiterRegEx) >= 2) {
            break;
        }

        capturedStart = wordDelimiterRegEx.match(word).capturedStart();

        if (capturedStart == 0) {
            break;
        } else if (capturedStart != -1) {
            // there are some symbols that we consider as word delimeters at the end of the word, so
            // just count till those
            previousWordEndPosition = currentWordStartPosition + capturedStart;

            break;
        }

        firstWord = false;
        broke = false;

        previousWordEndPosition = currentWordStartPosition + word.length();
    }

    // if a word ends with one of exceptionWordDelimiterRegEx chars, then remove it from selection
    if (exceptionWordDelimiterRegEx.match(word).capturedEnd() == 1 && !firstWord && !broke) {
        previousWordEndPosition -= 1;
    }

    return previousWordEndPosition;
}

void Spellchecker::getWordBoundaries(const QTextCursor& cursor, int* startingPosition, int* endingPosition)
{
    const static int MAX_LOOKUP = 10;

    (*startingPosition) = getWordStartingPosition(cursor, MAX_LOOKUP);
    (*endingPosition) = getWordEndingPosition(cursor, MAX_LOOKUP);
}

QList<QAction*> ContextMenuEnabledSpellchecker::getContextMenuSuggestions(QTextCursor cursor) const
{
    QList<QAction*> actions;

    if (!isEnabled() || !hasDictionarySet()) {
        return actions;
    }

    int left, right;
    getWordBoundaries(cursor, &left, &right);

    if (left == -1 || right == -1) {
        return actions;
    }

    cursor.setPosition(left);
    cursor.setPosition(right, QTextCursor::KeepAnchor);
    QString selectedWord = cursor.selectedText();

    // cursor.position() points to the end of the selected word
    // substract selectedWord.length() to get the start position
    if (selectedWord.count(exceptionWordDelimiterRegEx) != selectedWord.length() &&
            !skipRange(cursor.position() - selectedWord.length(), cursor.position()) &&
            !isCorrect(selectedWord)) {
        QStringList suggestions = suggest(selectedWord);

        if (!suggestions.isEmpty()) {
            QStringListIterator it(suggestions);
            for (int i = 0; i < 4 && it.hasNext(); i++) {
                QString suggestion = it.next();
                QAction* action = new QAction(suggestion, nullptr);
                connect(action, &QAction::triggered, [cursor, action]() mutable { cursor.insertText(action->text()); });
                actions << action;
            }
        }
    }

    return actions;
}

QActionGroup* ContextMenuEnabledSpellchecker::getContextMenuLanguages()
{
    QActionGroup* languageActions = new QActionGroup(nullptr);
    languageActions->setExclusive(true);

    const QList<QString> languages = getAvailableLanguages();

    const QString currentLanguage = getLanguage();

    for (const QString l : languages) {
        QAction* action = new QAction(l, languageActions);
        action->setCheckable(true);

        if (l == currentLanguage) {
            action->setChecked(true);
        }
    }
    connect(languageActions, &QActionGroup::triggered, [](QAction *a) {setLanguage(a->text());});

    return languageActions;
}

QAction* ContextMenuEnabledSpellchecker::getContextMenuEnableSpellchecking()
{
    QAction* action = new QAction(nullptr);
    action->setCheckable(true);
    action->setChecked(isEnabled());
    connect(action, &QAction::triggered, &ContextMenuEnabledSpellchecker::setEnabled);

    return action;
}
