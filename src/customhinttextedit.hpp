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

#ifndef CUSTOMHINTTEXTEDIT_HPP
#define CUSTOMHINTTEXTEDIT_HPP

#include <QTextEdit>

class CustomHintTextEdit : public QTextEdit
{
public:
    explicit CustomHintTextEdit(QWidget *parent, QSize startingSize);
    QSize sizeHint() const;

private:
    QSize startingSize;
    
};

#endif // CUSTOMHINTTEXTEDIT_HPP
