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

#ifndef ADDFRIENDDIALOG_HPP
#define ADDFRIENDDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>

class AddFriendDialog : public QDialog
{
    Q_OBJECT
public:
    AddFriendDialog(QWidget* parent);

    QString getUserId() const;
    QString getMessage() const;


private:
    QLineEdit* userIdEdit;
    QPlainTextEdit* messageEdit;

public slots:
    void accept();

};

#endif // ADDFRIENDDIALOG_HPP
