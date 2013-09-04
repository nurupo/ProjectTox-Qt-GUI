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
#include <QDebug>
#include <QApplication>
#include <QClipboard>

#include "emoticonmenu.hpp"

MessageLabel::MessageLabel(QWidget *parent) :
    QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);

    // Create contextmenu
    QAction *copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MessageLabel::copyPlainText);
    addAction(copyAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MessageLabel::setMessageId(int id)
{
    mId = id;
}

int MessageLabel::messageId() const
{
    return mId;
}

void MessageLabel::copyPlainText()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(EmoticonMenu::desmile(text()));
}
