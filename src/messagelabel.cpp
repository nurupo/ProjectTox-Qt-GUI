#include "messagelabel.hpp"

#include <QAction>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

#include "emoticonmenu.hpp"

MessageLabel::MessageLabel(QWidget *parent) :
    QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);

    // Create contextmenu
    QAction *copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MessageLabel::copyPlainText);
    addAction(copyAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MessageLabel::setMessageId(int id)
{
    mId = id;
}

int MessageLabel::messageId()
{
    return mId;
}

void MessageLabel::copyPlainText()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(EmoticonMenu::desmile(text()));
}
