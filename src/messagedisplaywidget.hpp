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

#ifndef MESSAGEDISPLAYWIDGET_HPP
#define MESSAGEDISPLAYWIDGET_HPP

#include <QScrollArea>

class QHBoxLayout;
class QVBoxLayout;

/*! New message display widget.
 *
 * CSS classes for styling
 * -----------------------
 *
 * Object         | Qt class           | CSS class
 * ---------------| -------------------|--------------
 * Timestamp      | QLabel             | msgTimestamp
 * Username       | ElideLabel         | msgUserName
 * Friendname     | ElideLabel         | msgFriendName
 * Message        | QLabel             | msgMessage
 * Seperator line | QFrame             | msgLine
 * Error message  | ElideLabel, QLabel | msgError
 */
class MessageDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit MessageDisplayWidget(QWidget *parent = 0);

    void appendMessage(const QString &name, const QString &message, int messageId = -1);
    void prependMessage(const QString &name, const QString &message, int messageId = -1);

private:
    QVBoxLayout *mainlayout;
    QString lastName;

    QString urlify(QString string);
    QHBoxLayout *createNewLine(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId);

private slots:
    void moveScrollBarToBottom(int min, int max);
    
};

#endif // MESSAGEDISPLAYWIDGET_HPP
