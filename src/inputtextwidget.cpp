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

#include "inputtextwidget.hpp"

#include <QKeyEvent>
#include <QDebug>
#include <QRegularExpression>
#include <QTextDocument>

InputTextWidget::InputTextWidget(QWidget* parent) :
    QTextEdit(parent)
{
    setMinimumSize(10, 50);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void InputTextWidget::setSmileyList(const EmoticonMenu::SmileyHash &list)
{
    smileyList = list;
}

void InputTextWidget::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier))
    {

        emit messageSent(desmile(toHtml()));
        clear();
    } else {
       QTextEdit::keyPressEvent(event);
    }
}

QString InputTextWidget::desmile(QString htmlText)
{
    // Replace smileys by their textual representation
    int i=0;
    QRegularExpression re("<img[\\s]+[^>]*?((alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?)|(src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?))((src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|(alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|>)");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while(match.hasMatch()){

        // Replace smiley
        htmlText.replace(match.captured(0), smileyList.value(match.captured(5)).first());

        // next match
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    QTextDocument doc;
    doc.setHtml(htmlText);
    return doc.toPlainText();
}
