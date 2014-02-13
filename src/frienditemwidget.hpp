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

#ifndef FRIENDITEMWIDGET_HPP
#define FRIENDITEMWIDGET_HPP

#include "status.hpp"
#include "copyableelidelabel.hpp"

#include <QLabel>
#include <QWidget>

class FriendItemWidget : public QWidget
{
    Q_OBJECT
public:
    FriendItemWidget(QWidget* parent);

private:
    QLabel* statusLabel;
    CopyableElideLabel* usernameLabel;
    CopyableElideLabel* statusMessageLabel;

    static const int LINE_SPACING_OFFSET = 2;

public slots:
    void setStatus(Status status);
    void setUsername(const QString& username);
    void setStatusMessage(const QString& statusMessage);

};

#endif // FRIENDITEMWIDGET_HPP
