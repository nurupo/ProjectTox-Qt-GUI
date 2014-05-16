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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QTextDocumentFragment>
#include <QList>

#include "smileypack.hpp"
#include "Settings/settings.hpp"

InputTextWidget::InputTextWidget(QWidget* parent) :
    QTextEdit(parent), spellchecker(document())
{
    setMinimumSize(10, 50);

    setPlaceholderText(tr("Type your text here..."));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &InputTextWidget::customContextMenuRequested, this, &InputTextWidget::showContextMenu);

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

    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());

    connect(actionUndo,  &QAction::triggered, this, &InputTextWidget::undo);
    connect(actionRedo,  &QAction::triggered, this, &InputTextWidget::redo);
    connect(actionCut,   &QAction::triggered, this, &InputTextWidget::cutPlainText);
    connect(actionCopy,  &QAction::triggered, this, &InputTextWidget::copyPlainText);
    connect(actionPaste, &QAction::triggered, this, &InputTextWidget::pastePlainText);

    connect(this, &InputTextWidget::undoAvailable, [this](bool enabled) {
        actionUndo->setEnabled(enabled);
    });
    connect(this, &InputTextWidget::redoAvailable, [this](bool enabled) {
        actionRedo->setEnabled(enabled);
    });
    connect(this, &InputTextWidget::copyAvailable, [this](bool enabled) {
        actionCut->setEnabled(enabled);
        actionCopy->setEnabled(enabled);
    });
}

/*! Handle keyboard events. */
void InputTextWidget::keyPressEvent(QKeyEvent* event)
{
    // Send message on Return
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier)) {
        // Prevents empty messages
        if (toPlainText().trimmed().isEmpty()) {
            return;
        }
        if (toPlainText().startsWith("/me ") ) {
            QString html = toHtml();
            html.remove(html.indexOf("/me "), 4);
            emit sendAction(Smileypack::desmilify(html));
        } else {
            emit sendMessage(Smileypack::desmilify(toHtml()));
        }
        // not only clears the text, but also removes undo/redo history
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
    if (!selection.isEmpty()) {
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
    if (!selection.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(Smileypack::desmilify(selection.toHtml()));
        textCursor().removeSelectedText();
    }
}

void InputTextWidget::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction(actionUndo);
    contextMenu.addAction(actionRedo);
    contextMenu.addSeparator();
    contextMenu.addAction(actionCut);
    contextMenu.addAction(actionCopy);
    contextMenu.addAction(actionPaste);

    actionPaste->setDisabled(QApplication::clipboard()->text().isEmpty());

    // get current selected word and - if neccessary - the suggested
    // words by the spellchecker
    // create a QAction for each suggested word and handle them after
    // the execution of the context menu
    QList<QAction*> actions;
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString selectedWord = cursor.selectedText();
    if (!spellchecker.isCorrect(selectedWord)) {
        QStringList suggestions;
        spellchecker.suggest(selectedWord, suggestions);

        if (!suggestions.isEmpty()) {
            contextMenu.addSeparator();
            QStringListIterator it(suggestions);
            while (it.hasNext()) {
                QString suggestion = it.next();
                QAction* action = new QAction(suggestion, this);
                action->setData(suggestion);
                contextMenu.addAction(action);
                actions.append(action);
            }
        }
    }


    QAction* selected = contextMenu.exec(globalPos);
    if (actions.contains(selected)) {
        cursor.insertText(selected->data().toString());
    }
    qDeleteAll(actions);
}
