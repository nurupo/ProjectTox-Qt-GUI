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

#include "smileypack.h"

#include <QTextDocument>
#include <QRegularExpression>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QDataStream>
#include "appinfo.hpp"
#include "Settings/settings.hpp"

Smileypack::Smileypack(QObject *parent) :
    QObject(parent)
{
    emoij = false;
}

Smileypack::Smileypack(const QByteArray &savedData, QObject *parent) :
    QObject(parent)
{
    restore(savedData);
}

void Smileypack::operator =(const Smileypack &other)
{
    list = other.getList();
    emoij = other.isEmoij();
    name = other.name;
    author= other.author;
    description= other.description;
    version= other.version;
    website= other.website;
    icon= other.icon;
}

/*! Replace all text smileys by images or emoij */
QString Smileypack::smile(QString text)
{
    Settings &settings = Settings::getInstance();
    Smileypack pack(settings.getSmileyPack());


    if (!pack.isEmoij()) {
        text = Smileypack::demoij(text);
    }

    // whlie smileys found to replace
    bool found;
    do {
        found = false;

        // Fill a map with positions of possible smileys
        QMap<int, QStringList> possibleTexts;
        for (const auto& pair : pack.getList()) {
            for (const QString& smileytext : pair.second) {
                int pos = text.indexOf(smileytext);
                if (pos > -1) {
                    possibleTexts.insertMulti(pos, {smileytext, pair.first});
                    found = true;
                }
            }
        }

        // check the first smiley alternative representations
        QMapIterator<int, QStringList> first(possibleTexts);
        if(first.hasNext())
        {
            first.next();
            int length = first.value().first().count();
            QString repSrt = first.value().first();
            int     repPos = first.key();
            QString repRep = first.value().at(1);

            QMapIterator<int, QStringList> i(possibleTexts);
            i.next();

            // Search for a longer smileyrepresentation at same position
            while (i.hasNext() && i.key() < first.key() + length) {
                i.next();

                // If there is a longer smileyrepresentation, use it
                if (i.value().count() > length) {
                    repPos = i.key();
                    repSrt = i.value().first();
                    repRep = i.value().at(1);
                }
            }

            // Replace found smiley
            if (pack.isEmoij()) {
                /*if (settings.isCurstomEmoijFont()) {
                    repRep = QString("<span style=\"font-family: '%1'; font-size: %2pt;\">%3</span>").arg(settings.getEmoijFont(), QString::number(settings.getEmoijSize()), repRep);
                }*/
                text.replace(repPos, repSrt.count(), repRep);
            }
            else {
                text.replace(repPos, repSrt.count(), QString("<img src=\"%1\" />").arg(repRep));
            }

        }
    } while (found);


    if (settings.isCurstomEmoijFont() && pack.isEmoij()) {
        text = Smileypack::resizeEmoij(text);
    }

    return text;
}

QString Smileypack::desmile(QString htmlText)
{
    Smileypack pack(Settings::getInstance().getSmileyPack());
    // Cancel if emoij
    if(pack.isEmoij()) {
        QTextDocument doc;
        doc.setHtml(htmlText);
        return doc.toPlainText();
    }

    // Replace smileys by their textual representation
    int i = 0;
    QRegularExpression re(R"_(<img[\s]+[^>]*?((alt*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?)|(src*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?))((src*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?>)|(alt*?[\s]?=[\s\"\']+(.*?)[\"\']+.*?>)|>))_");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while (match.hasMatch()) {
        // Replace smiley and match next
        for (const auto& pair : pack.getList()) {
            if (pair.first == match.captured(5)) {
                const QStringList& textSmilies = pair.second;
                if (textSmilies.isEmpty()) {
                    htmlText.remove(match.captured(0));
                } else {
                    htmlText.replace(match.captured(0), textSmilies.first());
                }
                break;
            }
        }
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    QTextDocument doc;
    doc.setHtml(htmlText);
    return doc.toPlainText();
}

/*! Replace Emoij by text strings */
QString Smileypack::demoij(QString text)
{
    for (const auto& pair : Smileypack::emoijList()) {
        const QStringList& textSmilies = pair.second;
        text.replace(pair.first, textSmilies.first().toHtmlEscaped());
    }
    return text;
}

QString Smileypack::resizeEmoij(QString text)
{
    Settings &settings = Settings::getInstance();

    // All Unicode 6.2 emoij "Emoticons" and a some of "Miscellaneous Symbols and Pictographs"
    // TODO find a regular expression for that
    QStringList foundEmoijs({"😀","😁","😂","😃","😄","😅","😆","😇","😈","😉","😊","😋","😌","😍","😎","😏","😐","😑","😒","😓","😔","😕","😖","😗","😘","😙","😚","😛","😜","😝","😞","😟","😠","😡","😢","😣","😤","😥","😦","😧","😨","😩","😪","😫","😬","😭","😮","😯","😰","😱","😲","😳","😴","😵","😶","😷","😸","😹","😺","😻","😼","😽","😾","😿","🙀","🙅","🙆","🙇","🙈","🙉","🙊","🙋","🙌","🙍","🙎","🙏","☺","☹","⚇","🐱","♥","☔","☀","♫","☕","★"});

    for(QString emo : foundEmoijs) {
        text.replace(emo, QString("<span style=\"font-family: '%1'; font-size: %2pt;\">%3</span>").arg(settings.getEmoijFont(), QString::number(settings.getEmoijSize()), emo));
    }

    return text;
}

const Smileypack::SmileyList Smileypack::emoijList()
{
    static const SmileyList tmpList =
    {
        // Skype Smileys
        {"☺", {":)",  ":-)"}},
        {"😞", {":(",  ":-("}},
        {"😄", {":D",  ":-D"}},
        {"😎", {"8)",  "8-)",  "B)",  "B-)"}},
        {"😲", {":O",  ":-O",  ":o",  ":-o"}},
        {"😉", {";)",  ";-)"}},
        {"😢", {";(",  ";-("}},
        {"😓", {"(:|",  "(:-|"}},
        {"😐", {":|",  ":-|"}},
        {"😚", {":*",  ":-*"}},
        {"😜", {":P",  ":-P",  ":p",  ":-p"}},
        // Blushing         :$
        {"😒", {":^)",  ":^-)"}},
        {"😪", {"|)",   "|-)"}},
        // Dull             |-(
        {"😍", {"(inlove)"}},
        {"😈", {"]:)", "]:-)", "(devil)"}},
        // Fingers crossed  (yn)
        // Yawn             (yawn)
        // Puking           (puke)
        // Doh!             (doh)
        {"😠", {">:(", ">:-(", "(angry)"}},
        // It wasn’t me!    (wasntme)
        // Party            (party)
        {"😰", {"(worry)"}},
        {"😏", {"(mm)"}},
        // Nerdy            (nerd)
        {"😷", {":x",  ":-x",  ":X",  ":-X"}},
        // Hi               (wave)
        // Facepalm         (facepalm)
        {"😇", {"O:)", "O:-)", "o:)", "o:-)", "(angel)"}},
        // ...
        {"♥", {"<3",  "(h)"}},
        // ...
        {"☔", {"(rain)"}},
        {"☀", {"(sun)"}},
        // Tumbleweed       (tumbleweed)
        {"♫", {"(music)"}},
        // ...
        {"☕", {"(coffee)"}},
        // ...
        {"★", {"(*)"}},
        // Additional smileys
        {"🐱", {":3"}}
    };
    return tmpList;
}

const Smileypack::SmileyList Smileypack::defaultList()
{
    static const SmileyList tmpList =
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
    return tmpList;
}

bool Smileypack::parseFile(const QString &filePath)
{
    // Open file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    themeFile = filePath;

    // Clear old data
    list.clear();
    emoij = false;
    name.clear();
    author.clear();
    description.clear();
    version.clear();
    website.clear();
    icon.clear();

    // Get pack folder path
    QFileInfo info(file);
    QString path = info.absolutePath() + QDir::separator();

    // Read lines
    ParserStates state = StateHead;
    while (!file.atEnd()) {
        processLine(file.readLine(), path, state);
    }

    // End parsing
    file.close();
    return true;
}

const QByteArray Smileypack::save()
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::WriteOnly);
    stream << (*this);
    return ret;
}

void Smileypack::restore(const QByteArray &array)
{
    QByteArray ar = array;
    QDataStream stream(&ar, QIODevice::ReadOnly);
    stream >> (*this);
}

void Smileypack::processLine(const QString &xLine, const QString &xPath, ParserStates &xState)
{
    // Trim spaces and exclue comment lines
    QString line = xLine.trimmed();
    if(line.startsWith('#') || line.isEmpty()) {
        return;
    }

    if (xState == StateHead) {

        // Switch on [theme]
        QRegularExpression rx("\\[(theme|smileys)\\]");
        QRegularExpressionMatch match = rx.match(line);
        if (match.hasMatch()) {
            xState = StateSmileys;
            return;
        }

        QString key   = line.section(QRegularExpression("\\s*\\=\\s*"), 0,0);
        QString value = line.section(QRegularExpression("\\s*\\=\\s*"), 1);

        if      (key == "Name")        name        = value;
        else if (key == "Author")      author      = value;
        else if (key == "Description") description = value;
        else if (key == "Version")     version     = value;
        else if (key == "Website")     website     = value;
        else if (key == "Icon")        icon        = value;
    }
    else if (xState == StateSmileys) {
        QString key   = line.section(QRegularExpression("\\s+"), 0,0);
        QString value = line.section(QRegularExpression("\\s+"), 1);

        list.append({xPath+key, value.split(QRegularExpression("\\s+"))});
    }
}


const QString &Smileypack::packDir()
{
    static QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+QDir::separator()+AppInfo::name+QDir::separator()+"smileys";
    return path;
}

QDataStream &operator<<(QDataStream &out, const Smileypack &pack)
{
    out << pack.getThemeFile() << pack.getName() << pack.getAuthor() << pack.getDescription() << pack.getVersion() << pack.getWebsite() << pack.getIcon() << pack.isEmoij();
    out << pack.getList();
    return out;
}

QDataStream &operator >>(QDataStream &in, Smileypack &pack)
{
    QString themefile;
    QString name;
    QString author;
    QString desc;
    QString version;
    QString website;
    QString icon;
    bool    emoij;
    Smileypack::SmileyList list;
    in >> themefile >> name >> author >> desc >> version >> website >> icon >> emoij >> list;

    pack.setThemeFile(themefile);
    pack.setName(name);
    pack.setAuthor(author);
    pack.setDescription(desc);
    pack.setVersion(version);
    pack.setWebsite(website);
    pack.setIcon(icon);
    pack.setEmoij(emoij);
    pack.setList(list);
    return in;
}
