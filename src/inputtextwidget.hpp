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

#ifndef INPUTTEXTWIDGET_HPP
#define INPUTTEXTWIDGET_HPP

#include <QTextEdit>

#include "emoticonmenu.h"

class InputTextWidget : public QTextEdit
{
    Q_OBJECT
public:
    InputTextWidget(QWidget* parent);

    void setSmileyList(const EmoticonMenu::SmileyList &list);

protected:
    void keyPressEvent(QKeyEvent* event);

signals:
    void messageSent(const QString& message);

private:
    EmoticonMenu::SmileyList smileyList;
    QString desmile(QString htmlText);

};

#endif // INPUTTEXTWIDGET_HPP
