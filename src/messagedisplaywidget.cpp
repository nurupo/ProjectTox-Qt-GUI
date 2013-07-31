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

#include <QDebug>
#include <QRegularExpression>
#include <QTime>

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
    document()->setDefaultStyleSheet("p {text-indent:-10px; margin-left:10px; margin-top:5; margin-bottom:0; white-space:pre-wrap;}");
}

void MessageDisplayWidget::showMessage(const QString& senderUsername, const QString& message)
{
    QString senderUsernameEscaped = senderUsername.toHtmlEscaped();
    QString timeEscaped = QString(getTime()).toHtmlEscaped();
    QString messageEscaped = message.toHtmlEscaped();

    QString text = QString("<p><b>%1</b> [%2]<br>%3</p>").arg(senderUsernameEscaped).arg(timeEscaped).arg(messageEscaped);
    urlify(text);
    append(text);
}

void MessageDisplayWidget::showFailedToSendMessage(const QString& message)
{
    QString messageEscaped = message.toHtmlEscaped();
    QString text = QString("<p><font color=\"red\" style=\"font-weight:bold\">Couldn't send following message:</font><br>%1</p>").arg(messageEscaped);
    urlify(text);
    append(text);
}

void MessageDisplayWidget::urlify(QString& string)
{
    string.replace(QRegularExpression("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QString MessageDisplayWidget::getTime() const
{
    return QTime::currentTime().toString("hh:mm:ss");
}
