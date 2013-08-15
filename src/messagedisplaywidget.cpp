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

#include "messagedisplaywidget.hpp"
#include "Settings/settings.hpp"

MessageDisplayWidget::MessageDisplayWidget(QWidget* parent) :
    QTextBrowser(parent)
{
    setMinimumSize(10, 50);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAcceptRichText(true);
    setOpenExternalLinks(true);
    setTextInteractionFlags((textInteractionFlags() & ~Qt::TextEditable) | Qt::LinksAccessibleByMouse);
    setUndoRedoEnabled(false);
    setCursorWidth(0);

    // Set messagestyle
    const Settings& settings = Settings::getInstance();
    if(settings.getMessageStyle() == "irc")
        messageStyle = new MessageIrcStyle();
    else
        messageStyle = new MessageDefaultStyle();
    messageStyle->setWidget(this);
}

void MessageDisplayWidget::showMessage(const QString& senderUsername, const QString& message)
{
    messageStyle->addMessage(senderUsername, message);
}

void MessageDisplayWidget::showFailedToSendMessage(const QString& message)
{
    messageStyle->addErrorMessage(tr("Couldn't send following message: %1").arg(message));
}
