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

#include "emoijfontsettings.h"
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontComboBox>
#include <QToolButton>
#include <QGroupBox>
#include <QDialogButtonBox>

EmoijFontSettings::EmoijFontSettings(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Emoij Settings"));

    selectGroup = new QGroupBox(tr("Override native font for Emoij"), this);
    selectGroup->setCheckable(true);

    fontCombobox = new QFontComboBox(selectGroup);

    sizeComboBox = new QComboBox(selectGroup);
    sizeComboBox->addItems({"6", "8", "10", "12", "14", "16", "18", "20"});
    sizeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    QToolButton *defaultButton = new QToolButton(selectGroup);
    defaultButton->setIcon(QIcon("://icons/arrow_undo.png"));
    defaultButton->setToolTip(tr("Reset to default."));
    defaultButton->setAutoRaise(true);
    connect(defaultButton, &QToolButton::clicked, this, &EmoijFontSettings::resetToDefault);

    QHBoxLayout *selectLayout = new QHBoxLayout(selectGroup);
    selectLayout->addWidget(fontCombobox);
    selectLayout->addWidget(sizeComboBox);
    selectLayout->addWidget(defaultButton);

    previewLabel = new QLabel(this);
    previewLabel->setText("☺ 😞 😄 😎 😲 😉 😜 😇 🐱");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setFrameShape(QFrame::StyledPanel);
    previewLabel->setFrameShadow(QFrame::Sunken);
    previewLabel->setMinimumSize(400, 50);
    previewLabel->setToolTip(tr("Unicode 6.1 preview"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(selectGroup);
    layout->addWidget(previewLabel);
    layout->addWidget(buttonBox);

    connect(selectGroup, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(fontCombobox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updatePreview()));
    connect(sizeComboBox, SIGNAL(currentIndexChanged(int)),  this, SLOT(updatePreview()));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &EmoijFontSettings::close);
}

bool EmoijFontSettings::isCustomEmoijFont()
{
    return selectGroup->isChecked();
}

void EmoijFontSettings::setCustomEmoijFont(bool x)
{
    selectGroup->setChecked(x);
}

QString EmoijFontSettings::getEmoijFont()
{
    return fontCombobox->currentFont().family();
}

void EmoijFontSettings::setEmoijFont(const QString &x)
{
    fontCombobox->setCurrentFont(QFont(x));
}

int EmoijFontSettings::getEmoijSize()
{
    return sizeComboBox->currentText().toInt();
}

void EmoijFontSettings::setEmoijSize(int x)
{
    sizeComboBox->setCurrentText(QString::number(x));
}

void EmoijFontSettings::updatePreview()
{
    QFont font = previewLabel->font();

    if (selectGroup->isChecked()) {
        font.setFamily(fontCombobox->currentFont().family());
        font.setPointSize(sizeComboBox->currentText().toInt());
    }
    else {
        QLabel label;
        font = label.font();
    }

    previewLabel->setFont(font);
}

void EmoijFontSettings::resetToDefault()
{
    fontCombobox->setCurrentFont(QFont("DejaVu Sans"));
    sizeComboBox->setCurrentText("12");
}
