#include "messagelabel.h"

#include <QAction>
#include <QDebug>
#include <QTextDocument>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>

MessageLabel::MessageLabel(QWidget *parent) :
    QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);

    // Create contextmenu
    QAction *copyAction = new QAction(tr("Copy"),       this);
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    connect(copyAction, &QAction::triggered, this, &MessageLabel::copyPlainText);
    addAction(copyAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MessageLabel::setSmileyList(const EmoticonMenu::SmileyHash &list)
{
    smileyHash = list;
}

QString MessageLabel::getPlainText()
{
    // Get message with images
    QTextDocument doc;
    doc.setHtml(text());
    QString htmlText = doc.toHtml();

    // Replace smileys by their textual representation
    int i=0;
    QRegularExpression re("<img[\\s]+[^>]*?((alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?)|(src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?))((src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|(alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|>)");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while(match.hasMatch()){

        // Replace smiley
        htmlText.replace(match.captured(0), smileyHash.value(match.captured(5)).first());

        // next match
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    doc.setHtml(htmlText);
    return doc.toPlainText();
}

void MessageLabel::copyPlainText()
{

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getPlainText());
}

