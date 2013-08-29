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

#ifndef EMOTICONMENU_H
#define EMOTICONMENU_H

#include <QMenu>

class QGridLayout;

/*! This Class represents a menu with all smileys for adding into text input field.
 * And it holds a hash of the smileys for translating image to text.
 */
class EmoticonMenu : public QMenu
{
    Q_OBJECT
public:
    typedef QHash<QString,QStringList> SmileyHash; //!< Hash of all smiley image pathes (key) and lists of their textual trpresantations (values).

    explicit EmoticonMenu(QWidget *parent = 0);

    static SmileyHash &getSmileyHash();
    static QString smile(QString text);
    static QString desmile(QString htmlText);
    
signals:
    void insertEmoticon(QString);
    
public slots:
    
private slots:
    void onEmoticonTriggered();

private:
    void addEmoticon(const QString &imgPath, const QStringList &texts);

    QGridLayout *layout;
};

#endif // EMOTICONMENU_H
