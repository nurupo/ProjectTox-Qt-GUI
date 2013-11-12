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

#ifndef EMOTICONMENU_HPP
#define EMOTICONMENU_HPP

#include <QMenu>

class QGridLayout;
class QWidgetAction;

/*! This Class represents a menu with all smileys for adding into text input field.
 * And it holds a hash of the smileys for translating image to text.
 */
class EmoticonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit EmoticonMenu(QWidget *parent = 0);

public slots:
    void updateEmoticons();

private:
    void addEmoticon(const QString &imgPath, const QStringList &texts, bool isEmoji = false);

    QGridLayout *layout;
    QWidgetAction *action;
    QWidget* actionDefaultWidget;

    const static int EMOTICONS_IN_A_ROW = 5;
    
signals:
    void insertEmoticon(QString);

private slots:
    void onEmoticonTriggered();

};

#endif // EMOTICONMENU_HPP
