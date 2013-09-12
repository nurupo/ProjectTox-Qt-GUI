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
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &InputTextWidget::customContextMenuRequested, this, &InputTextWidget::showContextMenu);
    showPlaceholder(true);

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

// FIXME: Replace Placeholder by Qt5.2 placeholder.
void InputTextWidget::focusInEvent(QFocusEvent *e)
{
    showPlaceholder(false);
    QTextEdit::focusInEvent(e);
}

// FIXME: Replace Placeholder by Qt5.2 placeholder.
void InputTextWidget::focusOutEvent(QFocusEvent *e)
{
    showPlaceholder(true);
    QTextEdit::focusInEvent(e);
}

QSize InputTextWidget::sizeHint() const
{
    return QSize(10, 50);
}

void InputTextWidget::insertHtml(const QString &text)
{
    showPlaceholder(false);
    QTextEdit::insertHtml(text);
}

/*! Copy text without images, but textual representations of the smileys. */
void InputTextWidget::copyPlainText()
{
    QTextDocumentFragment selection = textCursor().selection();
    if(!selection.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(EmoticonMenu::desmile(selection.toHtml()));
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
        clipboard->setText(EmoticonMenu::desmile(selection.toHtml()));
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

// FIXME: Replace Placeholder by Qt5.2 placeholder.
void InputTextWidget::showPlaceholder(bool show)
{
    if(show && toPlainText().isEmpty()) {
        QPalette pal = palette();
        pal.setColor(QPalette::Text, pal.color(QPalette::Mid));
        setPalette(pal);
        setText(tr("Type text here..."));
        placeholder = true;
    }
    else if(!show && placeholder) {
        QPalette pal = palette();
        pal.setColor(QPalette::Text, pal.color(QPalette::WindowText));
        setPalette(pal);
        clear();
        placeholder = false;
    }
}
