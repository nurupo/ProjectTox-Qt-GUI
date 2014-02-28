#include "clickable.hpp"
#include <QDesktopServices>
#include <QModelIndex>
#include <QUrl>


Clickable::Clickable(Clickable::Type type, quint16 start, quint16 length) :
    _type(type),
    _start(start),
    _length(length)
{}

void Clickable::activate(const QString &text) const
{
    if (!isValid())
        return;

    switch (type()) {
    case Clickable::Url:
        QDesktopServices::openUrl(QUrl::fromEncoded(text.toUtf8(), QUrl::TolerantMode));
        break;
    default:
        break;
    }
}

// NOTE: This method is not threadsafe and not reentrant!
//       (RegExps are not constant while matching, and they are static here for efficiency)
ClickableList ClickableList::fromString(const QString &str)
{
    // For matching URLs
    static QString scheme("(?:(?:mailto:|(?:[+.-]?\\w)+://)|www(?=\\.\\S+\\.))");
    static QString authority("(?:(?:[,.;@:]?[-\\w]+)+\\.?|\\[[0-9a-f:.]+\\])(?::\\d+)?");
    static QString urlChars("(?:[,.;:]*[\\w~@/?&=+$()!%#*{}\\[\\]\\|'^-])");
    static QString urlEnd("(?:>|[,.;:\"]*\\s|\\b|$)");

    static QRegExp regExp[] = {
        // URL
        // QRegExp(QString("((?:https?://|s?ftp://|irc://|mailto:|www\\.)%1+|%1+\\.[a-z]{2,4}(?:?=/%1+|\\b))%2").arg(urlChars, urlEnd)),
        QRegExp(QString("\\b(%1%2(?:/%3*)?)%4").arg(scheme, authority, urlChars, urlEnd), Qt::CaseInsensitive),

        // Channel name
        // We don't match for channel names starting with + or &, because that gives us a lot of false positives.
        //QRegExp("((?:#|![A-Z0-9]{5})[^,:\\s]+(?::[^,:\\s]+)?)\\b", Qt::CaseInsensitive)

        // TODO: Nicks, we'll need a filtering for only matching known nicknames further down if we do this
    };

    static const int regExpCount = 1; // number of regexps in the array above

    qint16 matches[] = { 0, 0, 0 };
    qint16 matchEnd[] = { 0, 0, 0 };

    ClickableList result;
    //QString str = data(ChatLineModel::DisplayRole).toString();

    qint16 idx = 0;
    qint16 minidx;
    int type = -1;

    do {
        type = -1;
        minidx = str.length();
        for (int i = 0; i < regExpCount; i++) {
            if (matches[i] < 0 || matchEnd[i] > str.length()) continue;
            if (idx >= matchEnd[i]) {
                matches[i] = regExp[i].indexIn(str, qMax(matchEnd[i], idx));
                if (matches[i] >= 0) matchEnd[i] = matches[i] + regExp[i].cap(1).length();
            }
            if (matches[i] >= 0 && matches[i] < minidx) {
                minidx = matches[i];
                type = i;
            }
        }
        if (type >= 0) {
            idx = matchEnd[type];
            QString match = str.mid(matches[type], matchEnd[type] - matches[type]);
            if (type == Clickable::Url && str.at(idx-1) == ')') { // special case: closing paren only matches if we had an open one
                if (!match.contains('(')) {
                    matchEnd[type]--;
                    match.chop(1);
                }
            }
            result.append(Clickable((Clickable::Type)type, matches[type], matchEnd[type] - matches[type]));
        }
    }
    while (type >= 0);
    return result;
}

Clickable ClickableList::atCursorPos(int idx)
{
    foreach(const Clickable &click, *this) {
        if (idx >= click.start() && idx < click.start() + click.length())
            return click;
    }
    return Clickable();
}
