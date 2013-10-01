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

#include "emoticonmenu.hpp"

#include <QGridLayout>
#include <QToolButton>
#include <QWidgetAction>

#include "smileypack.h"
#include "Settings/settings.hpp"

EmoticonMenu::EmoticonMenu(QWidget *parent) :
    QMenu(parent)
{
    action              = new QWidgetAction(this);
    actionDefaultWidget = new QWidget(this);
    layout              = new QGridLayout(actionDefaultWidget);
    updateEmoticons();

    connect(&Settings::getInstance(), &Settings::smileyPackChanged, this, &EmoticonMenu::updateEmoticons);
    connect(&Settings::getInstance(), &Settings::emojFontChanged, this, &EmoticonMenu::updateEmoticons);
}

void EmoticonMenu::updateEmoticons()
{
    // Delete old menu
    action->deleteLater();
    actionDefaultWidget->deleteLater();
    layout->deleteLater();

    // Create new menu
    action = new QWidgetAction(this);
    actionDefaultWidget = new QWidget(this);
    layout = new QGridLayout(actionDefaultWidget);
    layout->setMargin(1);
    layout->setSpacing(0);
    action->setDefaultWidget(actionDefaultWidget);
    addAction(action);

    // Add new pack
    Smileypack pack(Settings::getInstance().getSmileyPack());
    for (const auto& pair : pack.getList()) {
        addEmoticon(pair.first, pair.second, pack.isEmoij());
    }
}

void EmoticonMenu::addEmoticon(const QString &imgPath, const QStringList &texts, bool isEmoij)
{
    Settings &settings = Settings::getInstance();

    QToolButton *button = new QToolButton(this);
    if (isEmoij) {
        QFont font;
        font.setPixelSize(16);
        if (settings.isCurstomEmoijFont()) {
            font.setFamily(settings.getEmoijFont());
            button->setProperty("smiley", Smileypack::resizeEmoij(imgPath));
        } else {
            button->setProperty("smiley", imgPath);
        }
        button->setFont(font);
        button->setText(imgPath);
    }
    else {
        button->setIcon(QIcon(imgPath));
        button->setProperty("smiley", QString("<img src=\"%1\" />").arg(imgPath));
    }
    button->setAutoRaise(true);
    button->setToolTip(texts.first());

    connect(button, &QToolButton::clicked, this, &EmoticonMenu::onEmoticonTriggered);
    connect(button, &QToolButton::clicked, this, &EmoticonMenu::close);

    layout->addWidget(button, layout->count() / EMOTICONS_IN_A_ROW, layout->count() % EMOTICONS_IN_A_ROW);
}

/*! Signal sends the (first) textual form of the clicked smiley. */
void EmoticonMenu::onEmoticonTriggered()
{
    emit insertEmoticon("&nbsp;"+QObject::sender()->property("smiley").toString()+"&nbsp;");
}
