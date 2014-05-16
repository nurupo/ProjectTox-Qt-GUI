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

#ifndef SPELLCHECKER_HPP
#define SPELLCHECKER_HPP

#include <QSyntaxHighlighter>
#include <hunspell/hunspell.hxx>

class QRegExp;
class QTextCharFormat;

class Spellchecker : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Spellchecker(QTextDocument*);
    ~Spellchecker();

    bool isCorrect(const QString&);
    void suggest(const QString&, QStringList&);

protected:
    void highlightBlock(const QString& text);

private:
    Hunspell* hunspell;
    QRegExp  regEx;
    QTextCharFormat format;
};

#endif // SPELLCHECKER_HPP
