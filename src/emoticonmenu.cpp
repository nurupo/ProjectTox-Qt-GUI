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
#include <QDebug>
#include <QHash>
#include <QWidgetAction>
#include <QRegularExpression>
#include <QTextDocument>

const QHash<QString, QStringList> EmoticonMenu::smileyHash =
{
    {":/icons/emoticons/emotion_smile.png",    {":)", ":-)", ":o)"}},
    {":/icons/emoticons/emotion_sad.png",      {":(", ":-("}},
    {":/icons/emoticons/emotion_grin.png",     {":D", ":-D"}},
    {":/icons/emoticons/emotion_cool.png",     {"8)", "8-)"}},
    {":/icons/emoticons/emotion_suprised.png", {":O", ":-O"}},
    {":/icons/emoticons/emotion_wink.png",     {";)", ";-)"}},
    {":/icons/emoticons/emotion_cry.png",      {";(", ";-("}},
    {":/icons/emoticons/emotion_sweat.png",    {"(:|"}},
    {":/icons/emoticons/emotion_kiss.png",     {":*", ":-*"}},
    {":/icons/emoticons/emotion_tongue.png",   {":P", ":-P"}},
    {":/icons/emoticons/emotion_doubt.png",    {":^)", ":^-)"}},
    {":/icons/emoticons/emotion_love.png",     {"(inlove)"}},
    {":/icons/emoticons/emotion_evilgrin.png", {"]:)", "]:-)"}},
    {":/icons/emoticons/emotion_angel.png",    {"O:)", "O:-)", "o:)", "o:-)", "(angel)"}}
};

EmoticonMenu::EmoticonMenu(QWidget *parent) :
    QMenu(parent)
{
    QWidgetAction *action = new QWidgetAction(this);

    QWidget* actionDefaultWidget = new QWidget(this);

    layout = new QGridLayout(actionDefaultWidget);
    layout->setMargin(1);
    layout->setSpacing(0);

    action->setDefaultWidget(actionDefaultWidget);

    addAction(action);

    addEmoticons();
}

QString EmoticonMenu::smile(QString text)
{
    QHashIterator<QString, QStringList> i(smileyHash);
    while (i.hasNext()) {
        i.next();
        for (const QString& smileytext : i.value()) {
            text.replace(smileytext, QString("<img src=\"%1\" />").arg(i.key()));
        }
    }
    return text;
}

QString EmoticonMenu::desmile(QString htmlText)
{
    // Replace smileys by their textual representation
    int i = 0;
    QRegularExpression re(R"_(<img[\s]+[^>]*?((alt*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?)|(src*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?))((src*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?>)|(alt*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?>)|>))_");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while (match.hasMatch()) {
        // Replace smiley and match next
        QStringList textSmilies = smileyHash.value(match.captured(5));
        if (textSmilies.isEmpty()) {
            htmlText.remove(match.captured(0));
        } else {
            htmlText.replace(match.captured(0), textSmilies.first());
        }
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    QTextDocument doc;
    doc.setHtml(htmlText);
    return doc.toPlainText();
}

void EmoticonMenu::addEmoticons()
{
    QHashIterator<QString, QStringList> i(smileyHash);
    while (i.hasNext()) {
        i.next();
        addEmoticon(i.key(), i.value());
    }
}

void EmoticonMenu::addEmoticon(const QString &imgPath, const QStringList &texts)
{
    QToolButton *button = new QToolButton(this);
    button->setIcon(QIcon(imgPath));
    button->setAutoRaise(true);
    button->setToolTip(texts.first());
    button->setProperty("smiley", QString("<img src=\"%1\" />").arg(imgPath));
    connect(button, &QToolButton::clicked, this, &EmoticonMenu::onEmoticonTriggered);
    connect(button, &QToolButton::clicked, this, &EmoticonMenu::close);

    layout->addWidget(button, layout->count() / EMOTICONS_IN_A_ROW, layout->count() % EMOTICONS_IN_A_ROW);
}

/*! Signal sends the (first) textual form of the clicked smiley. */
void EmoticonMenu::onEmoticonTriggered()
{
    emit insertEmoticon(QObject::sender()->property("smiley").toString());
}
