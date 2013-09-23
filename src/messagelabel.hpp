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

#ifndef MESSAGELABEL_HPP
#define MESSAGELABEL_HPP

#include <QLabel>

class MessageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MessageLabel(QWidget *parent = 0);

    void setMessageId(int id);
    int messageId() const;

private:
    int mId;
    QAction *copyAction;
    QAction *copyAllAction;

private slots:
    void showContextMenu(const QPoint &pos);
};

#endif // MESSAGELABEL_HPP
