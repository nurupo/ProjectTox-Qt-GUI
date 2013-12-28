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
#include <QTextDocumentFragment>
#include <QApplication>
#include <QClipboard>
#include <QIcon>
#include <QAction>
#include <QMenu>

#include "smileypack.hpp"
#include "Settings/settings.hpp"

InputTextWidget::InputTextWidget(QWidget* parent) :
    QTextEdit(parent)
{
    setMinimumSize(10, 50);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &InputTextWidget::customContextMenuRequested, this, &InputTextWidget::showContextMenu);
    setPlaceholderText(tr("Type your text here..."));

    actionUndo  = new QAction(QIcon(":/icons/arrow_undo.png"), tr("Undo"), this);
    actionRedo  = new QAction(QIcon(":/icons/arrow_redo.png"), tr("Redo"), this);
    actionCut   = new QAction(QIcon(":/icons/cut.png"), tr("Cut"), this);
    actionCopy  = new QAction(QIcon(":/icons/page_copy.png"), tr("Copy"), this);
    actionPaste = new QAction(QIcon(":/icons/paste_plain.png"), tr("Paste"), this);
    actionUndo->setShortcut(QKeySequence::Undo);
    actionRedo->setShortcut(QKeySequence::Redo);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionUndo,  &QAction::triggered, this, &InputTextWidget::undo);
    connect(actionRedo,  &QAction::triggered, this, &InputTextWidget::redo);
    connect(actionCut,   &QAction::triggered, this, &InputTextWidget::cutPlainText);
    connect(actionCopy,  &QAction::triggered, this, &InputTextWidget::copyPlainText);
    connect(actionPaste, &QAction::triggered, this, &InputTextWidget::pastePlainText);
}

/*! Handle keyboard events. */
void InputTextWidget::keyPressEvent(QKeyEvent* event)
{
    // Send message on Return
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier)) {
        if (toPlainText().startsWith("/me ") ) {
            QString html = toHtml();
            html.remove(html.indexOf("/me "), 4);
            emit sendAction(Smileypack::desmilify(html));
        } else {
            emit sendMessage(Smileypack::desmilify(toHtml()));
        }
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
    if(!selection.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(Smileypack::desmilify(selection.toHtml()));
    }
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
    if(!selection.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(Smileypack::desmilify(selection.toHtml()));
        textCursor().removeSelectedText();
    }
}

void InputTextWidget::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = mapToGlobal(pos);

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionUndo);
    contextMenu->addAction(actionRedo);
    contextMenu->addSeparator();
    contextMenu->addAction(actionCut);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionPaste);

    // Disable cut and copy if nothing to copy
    if(textCursor().selection().isEmpty()) {
        actionCut->setDisabled(true);
        actionCopy->setDisabled(true);
    }

    // Disable paste if clipboard is empty
    if(QApplication::clipboard()->text().isEmpty()) {
        actionPaste->setDisabled(true);
    }

    contextMenu->exec(globalPos);
    contextMenu->deleteLater();

    actionCut->setEnabled(true);
    actionCopy->setEnabled(true);
    actionPaste->setEnabled(true);
}
