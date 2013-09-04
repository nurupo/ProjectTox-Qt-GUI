/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>
                  2013 by Martin Kröll <technikschlumpf@web.de>

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
#include <QTextDocumentFragment>
#include <QApplication>
#include <QClipboard>

#include "emoticonmenu.hpp"

InputTextWidget::InputTextWidget(QWidget* parent) :
    QTextEdit(parent)
{
    setMinimumSize(10, 50);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // TODO: Add more actions (Undo, Redo)
    // Create custom contextmenu
    QAction *actionCut   = new QAction(tr("Cut"), this);
    QAction *actionCopy  = new QAction(tr("Copy"), this);
    QAction *actionPaste = new QAction(tr("Paste"), this);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionCut,   &QAction::triggered, this, &InputTextWidget::cutPlainText);
    connect(actionCopy,  &QAction::triggered, this, &InputTextWidget::copyPlainText);
    connect(actionPaste, &QAction::triggered, this, &InputTextWidget::pastePlainText);
    addAction(actionCut);
    addAction(actionCopy);
    addAction(actionPaste);
}

/*! Handle keyboard events. */
void InputTextWidget::keyPressEvent(QKeyEvent* event)
{
    // Send message on Return
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier)) {
        emit sendMessage(EmoticonMenu::desmile(toHtml()));
        clear();

    // Override default shortcuts
    } else if (event == QKeySequence::Copy) {
        copyPlainText();
    } else if (event == QKeySequence::Cut) {
        cutPlainText();
    } else if (event == QKeySequence::Paste) {
        pastePlainText();

    // Normal text writing
    } else {
        QTextEdit::keyPressEvent(event);
    }
}

QSize InputTextWidget::sizeHint() const
{
    return QSize(10, 50);
}

/*! Copy text without images, but textual representations of the smileys. */
void InputTextWidget::copyPlainText()
{
    QTextDocumentFragment selection = textCursor().selection();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(EmoticonMenu::desmile(selection.toHtml()));
}

/*! Paste only plain text. */
void InputTextWidget::pastePlainText()
{
    QClipboard *clipboard = QApplication::clipboard();
    insertPlainText(clipboard->text());
}

/*! Cut text without images, but textual representations of the smileys. */
void InputTextWidget::cutPlainText()
{
    QTextDocumentFragment selection = textCursor().selection();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(EmoticonMenu::desmile(selection.toHtml()));
    textCursor().removeSelectedText();
}
