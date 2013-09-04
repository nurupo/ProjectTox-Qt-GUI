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

#include "messagedisplaywidget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QScrollBar>
#include <QRegularExpression>
#include <QFrame>
#include <QDebug>

#include "Settings/settings.hpp"
#include "elidelabel.hpp"
#include "messagelabel.hpp"
#include "emoticonmenu.hpp"

MessageDisplayWidget::MessageDisplayWidget(QWidget *parent) :
    QScrollArea(parent)
{
    // Scroll down on new Message
    QScrollBar* scrollbar = verticalScrollBar();
    connect(scrollbar, &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom, Qt::UniqueConnection);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    QWidget *widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setWidget(widget);

    mainlayout = new QVBoxLayout(widget);
    mainlayout->setSpacing(1);
    mainlayout->setMargin(1);
}

void MessageDisplayWidget::appendMessage(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId, bool isOur)
{
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom, Qt::UniqueConnection);
    QHBoxLayout *newLayout = createNewLine(name, message, messageId, isOur);
    mainlayout->addLayout(newLayout);
}

void MessageDisplayWidget::prependMessage(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId, bool isOur)
{
    disconnect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom);
    QHBoxLayout *newLayout = createNewLine(name, message, messageId, isOur);
    mainlayout->insertLayout(0, newLayout);
}

void MessageDisplayWidget::moveScrollBarToBottom(int min, int max)
{
    Q_UNUSED(min);
    this->verticalScrollBar()->setValue(max);
}

QString MessageDisplayWidget::urlify(QString string)
{
    return string.replace(QRegularExpression("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QHBoxLayout *MessageDisplayWidget::createNewLine(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId, bool isOur)
{
    ElideLabel *nameLabel = new ElideLabel(this);
    nameLabel->setMaximumWidth(50);
    nameLabel->setTextElide(true);
    nameLabel->setTextElideMode(Qt::ElideRight);
    nameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    nameLabel->setToolTip(name);
    nameLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

    MessageLabel *messageLabel = new MessageLabel(this);
    messageLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    // Message added to send que
    if (messageId) {
        messageLabel->setMessageId(messageId);
        messageLabel->setProperty("class", "msgMessage"); // for CSS styling
        messageLabel->setText(EmoticonMenu::smile(urlify(message.toHtmlEscaped())).replace('\n', "<br>"));

    // Error
    } else {
        QPalette errorPal;
        errorPal.setColor(QPalette::Foreground, Qt::red);
        messageLabel->setPalette(errorPal);
        messageLabel->setProperty("class", "msgError"); // for CSS styling
        messageLabel->setText(urlify(message.toHtmlEscaped()).prepend("<img src=\":/icons/error.png\" /> ").replace('\n', "<br>"));
        messageLabel->setToolTip(tr("Couldn't send the message!"));
    }

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    timeLabel->setForegroundRole(QPalette::Mid);
    timeLabel->setProperty("class", "msgTimestamp"); // for CSS styling
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignTop | Qt::AlignTrailing);
    timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));

    // Insert name if sender changed.
    if (lastMessageIsOurs != isOur || mainlayout->count() < 1) {
        nameLabel->setText(name);
        lastMessageIsOurs = isOur;

        if (isOur) {
            nameLabel->setForegroundRole(QPalette::Mid);
            nameLabel->setProperty("class", "msgUserName"); // for CSS styling
        } else {
            nameLabel->setProperty("class", "msgFriendName"); // for CSS styling
        }

        // Create line
        if (mainlayout->count() > 0) {
            QFrame *line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Plain);
            line->setForegroundRole(QPalette::Midlight);
            line->setProperty("class", "msgLine"); // for CSS styling
            mainlayout->addWidget(line);
        }
    }

    // Return new line
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->addWidget(nameLabel, 0, Qt::AlignTop);
    hlayout->addWidget(messageLabel, 0, Qt::AlignTop);
    hlayout->addWidget(timeLabel, 0, Qt::AlignTop);
    return hlayout;
}
