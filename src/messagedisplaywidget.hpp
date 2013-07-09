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

#ifndef MESSAGEDISPLAYWIDGET_HPP
#define MESSAGEDISPLAYWIDGET_HPP

#include <QDateTime>
#include <QTextBrowser>

class MessageDisplayWidget : public QTextBrowser
{
public:
    MessageDisplayWidget(QWidget* parent);
    void showMessage(const QString& senderUsername, const QString& message);

private:
    void urlify(QString& string);
    QString getTime() const;

};

#endif // MESSAGEDISPLAYWIDGET_HPP
