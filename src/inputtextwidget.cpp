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
#include <QApplication>
#include <QClipboard>
#include <QTextDocumentFragment>

InputTextWidget::InputTextWidget(QWidget* parent) :
    QTextEdit(parent)
{
    setMinimumSize(10, 50);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // Create custom contextmenu
    QAction *actionCopy  = new QAction(tr("Copy"), this);
    QAction *actionCut   = new QAction(tr("Cut"), this);
    QAction *actionPaste = new QAction(tr("Paste"), this);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionCut->setShortcut(QKeySequence::Cut);
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionCopy,  &QAction::triggered, this, &InputTextWidget::copyPlainText);
    connect(actionCut,   &QAction::triggered, this, &InputTextWidget::cutPlainText);
    connect(actionPaste, &QAction::triggered, this, &InputTextWidget::pastePlainText);
    addAction(actionCopy);
    addAction(actionCut);
    addAction(actionPaste);
}

/*! Set smileyhash for smiley image <-> text replacing. */
void InputTextWidget::setSmileyList(const EmoticonMenu::SmileyHash &list)
{
    smileyList = list;
}

/*! Handle keyboard events. */
void InputTextWidget::keyPressEvent(QKeyEvent* event)
{
    // Send message on Return
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier))
    {
        emit messageSent(desmile(toHtml()));
        clear();
    }

    // Override default shortcuts
    else if(event == QKeySequence::Copy)
        copyPlainText();
    else if(event == QKeySequence::Cut)
        cutPlainText();
    else if(event == QKeySequence::Paste)
        pastePlainText();

    // normal text writing
    else
       QTextEdit::keyPressEvent(event);
}

/*! Replace smiley images by their textual representations. */
QString InputTextWidget::desmile(QString htmlText)
{
    // Replace smileys by their textual representation
    int i=0;
    QRegularExpression re("<img[\\s]+[^>]*?((alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?)|(src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?))((src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|(alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|>)");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while(match.hasMatch())
    {
        // Replace smiley and match next
        htmlText.replace(match.captured(0), smileyList.value(match.captured(5)).first());
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    QTextDocument doc;
    doc.setHtml(htmlText);
    return doc.toPlainText();
}

/*! Copy texy without images, but textual representations of the smileys. */
void InputTextWidget::copyPlainText()
{
    QTextDocumentFragment selection = textCursor().selection();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(desmile(selection.toHtml()));
}

/*! Paste only plain text. */
void InputTextWidget::pastePlainText()
{
    QClipboard *clipboard = QApplication::clipboard();
    insertPlainText(clipboard->text());
}

/*! Cut texy without images, but textual representations of the smileys. */
void InputTextWidget::cutPlainText()
{
    QTextDocumentFragment selection = textCursor().selection();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(desmile(selection.toHtml()));
    textCursor().removeSelectedText();
}
