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

#include "smileypack.hpp"

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
    emoji = false;
}

Smileypack::Smileypack(const QByteArray &savedData, QObject *parent) :
    QObject(parent)
{
    restore(savedData);
}

void Smileypack::operator =(const Smileypack &other)
{
    list = other.getList();
    emoji = other.isEmoji();
    name = other.name;
    author = other.author;
    description = other.description;
    version = other.version;
    website = other.website;
    icon = other.icon;
}

/*! Replace all text smileys by images or emoji */
QString Smileypack::smilify(QString text)
{
    Settings &settings = Settings::getInstance();
    Smileypack pack(settings.getSmileyPack());


    if (!pack.isEmoji()) {
        text = Smileypack::deemojify(text);
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
        if (first.hasNext())
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
            if (pack.isEmoji()) {
                /*if (settings.isCurstomEmojiFont()) {
                    repRep = QString("<span style=\"font-family: '%1'; font-size: %2pt;\">%3</span>").arg(settings.getEmojiFont(), QString::number(settings.getEmojiSize()), repRep);
                }*/
                text.replace(repPos, repSrt.count(), repRep);
            }
            else {
                text.replace(repPos, repSrt.count(), QString("<img src=\"%1\" />").arg(repRep));
            }

        }
    } while (found);


    if (settings.isCurstomEmojiFont() && pack.isEmoji()) {
        text = Smileypack::resizeEmoji(text);
    }

    return text;
}

QString Smileypack::desmilify(QString htmlText)
{
    Smileypack pack(Settings::getInstance().getSmileyPack());
    // Cancel if emoji
    if (pack.isEmoji()) {
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

/*! Replace Emoji by text strings */
QString Smileypack::deemojify(QString text)
{
    for (const auto& pair : Smileypack::emojiList()) {
        const QStringList& textSmilies = pair.second;
        text.replace(pair.first, textSmilies.first().toHtmlEscaped());
    }
    return text;
}

QString Smileypack::resizeEmoji(QString text)
{
    Settings &settings = Settings::getInstance();

    // All Unicode 6.2 emoji "Emoticons" and a some of "Miscellaneous Symbols and Pictographs"
    // TODO find a regular expression for that
    // nurupo: that will do `text.replace(QRegularExpression("([\\x{1F600}-\\x{1F64F}])"), QString("<span style=\"font-family: '%1'; font-size: %2pt;\">\\1</span>").arg(settings.getEmojiFont(), QString::number(settings.getEmojiSize())));`
    //         except that you used symbols outside the Emoticons range (1F600-1F64F), so we need count for the too
    QStringList foundEmojis({"😀","😁","😂","😃","😄","😅","😆","😇","😈","😉","😊","😋","😌","😍","😎","😏","😐","😑","😒","😓","😔","😕","😖","😗","😘","😙","😚","😛","😜","😝","😞","😟","😠","😡","😢","😣","😤","😥","😦","😧","😨","😩","😪","😫","😬","😭","😮","😯","😰","😱","😲","😳","😴","😵","😶","😷","😸","😹","😺","😻","😼","😽","😾","😿","🙀","🙅","🙆","🙇","🙈","🙉","🙊","🙋","🙌","🙍","🙎","🙏","☺","☹","⚇","🐱","♥","☔","☀","♫","☕","★"});

    for (QString emo : foundEmojis) {
        text.replace(emo, QString("<span style=\"font-family: '%1'; font-size: %2pt;\">%3</span>").arg(settings.getEmojiFontFamily(), QString::number(settings.getEmojiFontPointSize()), emo));
    }

    return text;
}

const Smileypack::SmileyList Smileypack::emojiList()
{
    static const SmileyList tmpList =
    {
        // Smileys
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
    emoji = false;
    name.clear();
    author.clear();
    description.clear();
    version.clear();
    website.clear();
    icon.clear();

    // Get pack folder path
    QFileInfo info(file);
    QString path = info.absolutePath() + '/';

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
    if (line.startsWith('#') || line.isEmpty()) {
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
    static QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + "smileys";
    return path;
}

const QString &Smileypack::defaultPackDirRelative()
{
    static QString path = QString("../resources") + '/' + "icons";
    return path;
}

QDataStream &operator<<(QDataStream &out, const Smileypack &pack)
{
    out << pack.getThemeFile() << pack.getName() << pack.getAuthor() << pack.getDescription() << pack.getVersion() << pack.getWebsite() << pack.getIcon() << pack.isEmoji();
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
    bool    emoji;
    Smileypack::SmileyList list;
    in >> themefile >> name >> author >> desc >> version >> website >> icon >> emoji >> list;

    pack.setThemeFile(themefile);
    pack.setName(name);
    pack.setAuthor(author);
    pack.setDescription(desc);
    pack.setVersion(version);
    pack.setWebsite(website);
    pack.setIcon(icon);
    pack.setEmoji(emoji);
    pack.setList(list);
    return in;
}
