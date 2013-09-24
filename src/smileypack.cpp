#include "smileypack.h"

#include <QTextDocument>
#include <QRegularExpression>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include "appinfo.hpp"

const Smileypack::SmileyList Smileypack::emoijList =
{
    {"😃", {":)",  ":-)"}},
    {"😚", {":*",  ":-*"}},
    {"😇", {"O:)", "O:-)", "o:)", "o:-)", "(angel)"}},
    {"😎", {"8)",  "8-)",  "B)",  "B-)"}}
};

Smileypack::Smileypack(QObject *parent) :
    QObject(parent)
{
    list = emoijList;
    emoij = true;

    name = "Emoij";
    author = "Unicode 6.1";
    description = "Emoij is a Unicode block containing graphic representations of faces, which are often associated with classic emoticons.";
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

Smileypack &Smileypack::currentPack()
{
    static Smileypack currentPack;
    return currentPack;
}

QString Smileypack::smile(QString text)
{
    for (const auto& pair : currentPack().getList()) {
        for (const QString& smileytext : pair.second) {
            if (currentPack().isEmoij()) {
                text.replace(smileytext, pair.first);
            }
            else {
                text.replace(smileytext, QString("<img src=\"%1\" />").arg(pair.first));
            }
        }
    }
    return text;
}

QString Smileypack::desmile(QString htmlText)
{
    // Cancel if emoij
    if(currentPack().isEmoij()) {
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
        for (const auto& pair : currentPack().getList()) {
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

bool Smileypack::parseFile(const QString &filePath)
{
    // Open file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

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
