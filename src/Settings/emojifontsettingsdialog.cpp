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

#include "emojifontsettingsdialog.hpp"
#include "emojifontcombobox.hpp"

#include <QApplication>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

EmojiFontSettingsDialog::EmojiFontSettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Emoji Settings"));

    selectGroup = new QGroupBox(tr("Override native font for Emoji"), this);
    selectGroup->setCheckable(true);

    fontComboBox = new EmojiFontComboBox(selectGroup);

    sizeComboBox = new QSpinBox(selectGroup);
    sizeComboBox->setRange(4, 72);
    sizeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    QToolButton *defaultButton = new QToolButton(selectGroup);
    defaultButton->setIcon(QIcon("://icons/arrow_undo.png"));
    defaultButton->setToolTip(tr("Reset"));
    defaultButton->setAutoRaise(true);
    connect(defaultButton, &QToolButton::clicked, this, &EmojiFontSettingsDialog::resetToDefault);

    QHBoxLayout *selectLayout = new QHBoxLayout(selectGroup);
    selectLayout->addWidget(fontComboBox);
    selectLayout->addWidget(sizeComboBox);
    selectLayout->addWidget(defaultButton);

    previewLabel = new QLabel(this);
    previewLabel->setText("☺ 😞 😄 😎 😲 😉 😜 😇 🐱");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setFrameShape(QFrame::StyledPanel);
    previewLabel->setFrameShadow(QFrame::Sunken);
    previewLabel->setMinimumSize(400, 50);
    previewLabel->setToolTip(tr("Emoticons preview"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(selectGroup);
    layout->addWidget(previewLabel);
    layout->addWidget(buttonBox);
    layout->addStretch(0);

    connect(selectGroup,    SIGNAL(toggled(bool)),              this, SLOT(updatePreview()));
    connect(fontComboBox,   SIGNAL(currentIndexChanged(int)),   this, SLOT(updatePreview()));
    connect(sizeComboBox,   SIGNAL(valueChanged(int)),          this, SLOT(updatePreview()));
    connect(buttonBox,      &QDialogButtonBox::accepted,        this, &EmojiFontSettingsDialog::close);
}

bool EmojiFontSettingsDialog::useCustomFont() const
{
    return selectGroup->isChecked();
}

void EmojiFontSettingsDialog::setUseCustomFont(bool use)
{
    selectGroup->setChecked(use);
}

QString EmojiFontSettingsDialog::getFontFamily() const
{
    return fontComboBox->itemText(fontComboBox->currentIndex());
}

void EmojiFontSettingsDialog::setFontFamily(QString fontFamily)
{
    fontComboBox->setCurrentIndex(fontComboBox->findText(fontFamily));
    defaultFontFamily = fontFamily;
}

int EmojiFontSettingsDialog::getFontPointSize() const
{
    return sizeComboBox->value();
}

void EmojiFontSettingsDialog::setFontPointSize(int pointSize)
{
    sizeComboBox->setValue(pointSize);
    defaultFontPointSize = pointSize;
}

void EmojiFontSettingsDialog::updatePreview()
{
    QFont font = previewLabel->font();

    if (selectGroup->isChecked()) {
        font.setFamily(fontComboBox->itemText(fontComboBox->currentIndex()));
        font.setPointSize(sizeComboBox->value());
    } else {
        font = QApplication::font();
    }

    previewLabel->setFont(font);
}

void EmojiFontSettingsDialog::resetToDefault()
{
    fontComboBox->setCurrentIndex(fontComboBox->findText(defaultFontFamily));
    sizeComboBox->setValue(defaultFontPointSize);
}
