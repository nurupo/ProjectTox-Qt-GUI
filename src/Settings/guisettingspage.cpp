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

#include "guisettingspage.hpp"

#include "settings.hpp"
#include "appinfo.hpp"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QRegularExpression>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>

#include "smileypack.hpp"
#include "emojifontsettingsdialog.hpp"

GuiSettingsPage::GuiSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void GuiSettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(buildAnimationGroup());
    layout->addWidget(buildSmileypackGroup());
    layout->addStretch(0);
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    enableAnimationCheckbox->setChecked(settings.isAnimationEnabled());

    emojiSettings->setUseCustomFont(settings.isCurstomEmojiFont());
    emojiSettings->setFontFamily(settings.getEmojiFontFamily());
    emojiSettings->setFontPointSize(settings.getEmojiFontPointSize());

    // Insert Emoji pack
    Smileypack emojiPack;
    emojiPack.setName("Emoji");
    emojiPack.setAuthor("Unicode 6.1");
    emojiPack.setDescription("Emoji is a Unicode block containing graphic representations of faces, which are often associated with classic emoticons.");
    emojiPack.setList(Smileypack::emojiList());
    emojiPack.setEmoji(true);
    smileypackCombobox->addItem("☺ "+emojiPack.getName(), emojiPack.save());

    // Insert Default pack
    Smileypack defaultPack;
    defaultPack.setName("TOX Qt GUI Smileys");
    defaultPack.setAuthor("FatCow");
    defaultPack.setDescription("TODO: Design a default smileypack for TOX Qt GUI.");
    defaultPack.setIcon(":/icons/emoticons/emotion_smile.png");
    defaultPack.setList(Smileypack::defaultList());
    smileypackCombobox->addItem(QIcon(defaultPack.getIcon()), defaultPack.getName(), defaultPack.save());

    // Insert smileypacks
    searchSmileyPacks();

    // Load smileypack
    int index = smileypackCombobox->findData(settings.getSmileyPack());
    if (index < 0) {
        index = 1;
    }
    smileypackCombobox->setCurrentIndex(index);
}

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setAnimationEnabled(enableAnimationCheckbox->isChecked());

    settings.setSmileyPack(smileypackCombobox->itemData(smileypackCombobox->currentIndex()).toByteArray());
    settings.setCurstomEmojiFont(emojiSettings->useCustomFont());
    settings.setEmojiFontFamily(emojiSettings->getFontFamily());
    settings.setEmojiFontPointSize(emojiSettings->getFontPointSize());
}

QGroupBox *GuiSettingsPage::buildAnimationGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smooth animation"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    enableAnimationCheckbox = new QCheckBox(tr("Enable animation"), group);

    layout->addWidget(enableAnimationCheckbox);
    return group;
}

QGroupBox *GuiSettingsPage::buildSmileypackGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smileypack"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    smileypackCombobox = new QComboBox(group);
    connect(smileypackCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSmileypackDetails(int)));

    emojiSettings = new EmojiFontSettingsDialog(group);
    emojiButton = new QToolButton(group);
    emojiButton->setText("...");
    connect(emojiButton, &QToolButton::clicked, emojiSettings, &EmojiFontSettingsDialog::exec);

    QHBoxLayout *selectLayout = new QHBoxLayout;
    selectLayout->addWidget(smileypackCombobox);
    selectLayout->addWidget(emojiButton);

    smileypackDescLabel = new QLabel(group);
    smileypackDescLabel->setWordWrap(true);
    smileypackDescLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    smileypackDescLabel->setOpenExternalLinks(true);

    layout->addLayout(selectLayout);
    layout->addWidget(smileypackDescLabel);
    return group;
}

void GuiSettingsPage::searchSmileyPacks()
{
    // Go to smiley pack folder
    QDir dir(Smileypack::packDir());
    if(!dir.mkpath(Smileypack::packDir()))
        return;

    // Go through all packs
    dir.setFilter(QDir::Dirs|QDir::NoDot|QDir::NoDotDot);
    QDirIterator it(dir);
    while (it.hasNext()) {
        it.next();

        // Check theme file
        QFileInfo f(it.filePath() + '/' + "theme");
        if (!f.exists()) {
            continue;
        }

        // Parse theme file
        Smileypack newPack;
        if (!newPack.parseFile(f.absoluteFilePath())) {
            continue;
        }

        // Add new pack to combobox
        QVariant data(newPack.save());
        smileypackCombobox->addItem(QIcon(it.filePath() + '/' + newPack.getIcon()), newPack.getName(), data);
    }
}

void GuiSettingsPage::updateSmileypackDetails(int index)
{
    Smileypack pack(smileypackCombobox->itemData(index).toByteArray());

    QString version = pack.getVersion();
    if (!version.isEmpty()) {
        version.prepend(" v");
    }
    QString website = pack.getWebsite();
    if (!website.isEmpty()) {
        website = QString("<br><a href=\"%1\">%1</a>").arg(website);
    }
    QString desc = tr("<b>%1</b>%2 by %3<br>\"<i>%4</i>\"%5").arg(pack.getName(), version, pack.getAuthor(), pack.getDescription(), website);
    smileypackDescLabel->setText(desc);

    if (pack.isEmoji()) {
        emojiButton->setVisible(true);

    }
    else {
        emojiButton->setVisible(false);
    }
}
