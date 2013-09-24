#include "smileypack.h"

#include <QTextDocument>
#include <QRegularExpression>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QDataStream>
#include "appinfo.hpp"
#include "Settings/settings.hpp"

const Smileypack::SmileyList Smileypack::emoijList =
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
    {"♥", {"(h)"}},
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

Smileypack::Smileypack(QObject *parent) :
    QObject(parent)
{
    list = emoijList;
    emoij = true;

    name = "Emoij";
    author = "Unicode 6.1";
    description = "Emoij is a Unicode block containing graphic representations of faces, which are often associated with classic emoticons.";
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

QString Smileypack::smile(QString text)
{
    Smileypack pack(Settings::getInstance().getSmileyPack());
    for (const auto& pair : pack.getList()) {
        for (const QString& smileytext : pair.second) {
            if (pack.isEmoij()) {
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
    return ret.toBase64();
}

void Smileypack::restore(const QByteArray &array)
{
    QByteArray tmp = QByteArray::fromBase64(array);
    QDataStream stream(&tmp, QIODevice::ReadOnly);
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
