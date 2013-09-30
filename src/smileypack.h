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

#ifndef SMILEYPACK_H
#define SMILEYPACK_H

#include <QObject>
#include <QPair>
#include <QStringList>


class Smileypack : public QObject
{
    Q_OBJECT
public:
    typedef QList<QPair<QString, QStringList>> SmileyList;

    explicit Smileypack(QObject *parent = 0);
    explicit Smileypack(const QByteArray &savedData, QObject *parent = 0);
    void operator=(const Smileypack &other);

    bool parseFile(const QString &filePath);

    const QByteArray save();
    void restore(const QByteArray &array);

    const QString &getThemeFile() const             { return themeFile;   }
    void           setThemeFile(const QString &x)   { themeFile = x;      }
    const QString &getName() const                  { return name;        }
    void           setName(const QString &x)        { name = x;           }
    const QString &getAuthor() const                { return author;      }
    void           setAuthor(const QString &x)      { author = x;         }
    const QString &getDescription() const           { return description; }
    void           setDescription(const QString &x) { description = x;    }
    const QString &getVersion() const               { return version;     }
    void           setVersion(const QString &x)     { version = x;        }
    const QString &getWebsite() const               { return website;     }
    void           setWebsite(const QString &x)     { website = x;        }
    const QString &getIcon() const                  { return icon;        }
    void           setIcon(const QString &x)        { icon = x;           }
    const SmileyList &getList() const               { return list;        }
    void              setList(const SmileyList &x)  { list = x;           }
    bool  isEmoij() const                           { return emoij;       }
    void setEmoij(bool x)                           { emoij = x;          }

    static const QString& packDir();
    static QString smile(QString text);
    static QString desmile(QString htmlText);
    static QString demoij(QString text);
    static QString resizeEmoij(QString text);
    static const SmileyList emoijList();
    static const SmileyList defaultList();

private:
    QString themeFile;
    QString name;
    QString author;
    QString description;
    QString version;
    QString website;
    QString icon;
    SmileyList list;
    bool emoij;

    // Parser functions
    enum ParserStates{
        StateHead,
        StateSmileys
    };
    void processLine(const QString &xLine, const QString &xPath, ParserStates &xState);
};

QDataStream &operator<<(QDataStream &out, const Smileypack &pack);
QDataStream &operator>>(QDataStream &in, Smileypack &pack);

#endif // SMILEYPACK_H
