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

#ifndef EMOIJFONTSETTINGS_H
#define EMOIJFONTSETTINGS_H

#include <QDialog>

class QGroupBox;
class QFontComboBox;
class QComboBox;
class QLabel;

class EmoijFontSettings : public QDialog
{
    Q_OBJECT
public:
    explicit EmoijFontSettings(QWidget *parent = 0);

    bool    isCustomEmoijFont();
    void    setCustomEmoijFont(bool x);
    QString getEmoijFont();
    void    setEmoijFont(const QString &x);
    int     getEmoijSize();
    void    setEmoijSize(int x);

private slots:
    void updatePreview();
    void resetToDefault();
    
private:
    QGroupBox *selectGroup;
    QFontComboBox *fontCombobox;
    QComboBox *sizeComboBox;
    QLabel    *previewLabel;
};

#endif // EMOIJFONTSETTINGS_H
