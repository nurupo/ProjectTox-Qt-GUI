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

#ifndef EMOJIFONTSETTINGSDIALOG_HPP
#define EMOJIFONTSETTINGSDIALOG_HPP

#include <QDialog>

class EmojiFontComboBox;
class QGroupBox;
class QLabel;
class QSpinBox;

class EmojiFontSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EmojiFontSettingsDialog(QWidget *parent = 0);

    bool    useCustomFont() const;
    void    setUseCustomFont(bool use);
    QString getFontFamily() const;
    void    setFontFamily(QString fontFamily);
    int     getFontPointSize() const;
    void    setFontPointSize(int pointSize);
    
private:
    QGroupBox *selectGroup;
    EmojiFontComboBox *fontComboBox;
    QSpinBox *sizeComboBox;
    QLabel   *previewLabel;
    QString  defaultFontFamily;
    int      defaultFontPointSize;

private slots:
    void updatePreview();
    void resetToDefault();

};

#endif // EMOJIFONTSETTINGSDIALOG_HPP
