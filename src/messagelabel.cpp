/*
    Copyright (C) 2013 by Martin Kröll <technikschlumpf@web.de>

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

#include "messagelabel.hpp"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>

#include "smileypack.hpp"

MessageLabel::MessageLabel(QWidget *parent) :
    QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

    // Create contextmenu
    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [this]()
    {
        QApplication::clipboard()->setText(Smileypack::desmilify(selectedText()));
    });

    copyAllAction = new QAction(QIcon(":/icons/page_copy.png"), tr("Copy Message"), this);
    connect(copyAllAction, &QAction::triggered, [this]()
    {
        QApplication::clipboard()->setText(Smileypack::desmilify(text()));
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &MessageLabel::customContextMenuRequested, this, &MessageLabel::showContextMenu);
}

void MessageLabel::setMessageId(int id)
{
    mId = id;
}

int MessageLabel::messageId() const
{
    return mId;
}

void MessageLabel::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = mapToGlobal(pos);

    QMenu *contextMenu = new QMenu(this);

    contextMenu->addAction(copyAction);
    contextMenu->addAction(copyAllAction);

    copyAction->setEnabled(hasSelectedText());

    contextMenu->exec(globalPos);
    contextMenu->deleteLater();
}
