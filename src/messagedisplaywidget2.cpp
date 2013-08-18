#include "messagedisplaywidget2.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QScrollBar>
#include <QRegularExpression>
#include <QFrame>
#include <QDebug>

#include "Settings/settings.hpp"
#include "elidelabel.hpp"

MessageDisplayWidget2::MessageDisplayWidget2(QWidget *parent) :
    QScrollArea(parent)
{
    // Scroll down on new Message
    QScrollBar* scrollbar = verticalScrollBar();
    connect(scrollbar, &QScrollBar::rangeChanged, this, &MessageDisplayWidget2::moveScrollBarToBottom);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    QWidget *widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setWidget(widget);

    mainlayout = new QVBoxLayout(widget);
    mainlayout->setSpacing(1);
}

void MessageDisplayWidget2::setSmileyList(const EmoticonMenu::SmileyList &list)
{
    smileyList = list;
}

void MessageDisplayWidget2::appendMessage(const QString &name, const QString &message/*, const QString &timestamp, const QString &messageId*/)
{
    // Setup new row
    ElideLabel *nameLabel = new ElideLabel(this);
    nameLabel->setMaximumWidth(50);
    nameLabel->setTextElide(true);
    nameLabel->setTextElideMode(Qt::ElideRight);
    nameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    nameLabel->setToolTip(name);
    nameLabel->setAlignment(Qt::AlignLeading |
                            Qt::AlignLeft    |
                            Qt::AlignTop);

    QLabel *messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);
    messageLabel->setOpenExternalLinks(true);
    messageLabel->setProperty("class", "msgMessage"); // for CSS styling
    messageLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard |
                                          Qt::LinksAccessibleByMouse    |
                                          Qt::TextBrowserInteraction    |
                                          Qt::TextSelectableByKeyboard  |
                                          Qt::TextSelectableByMouse);
    messageLabel->setAlignment(Qt::AlignLeading |
                               Qt::AlignLeft    |
                               Qt::AlignTop);

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    timeLabel->setForegroundRole(QPalette::Mid);
    timeLabel->setProperty("class", "msgTimestamp"); // for CSS styling
    timeLabel->setAlignment(Qt::AlignRight |
                            Qt::AlignTop   |
                            Qt::AlignTrailing);



    // Insert new message
    if(name != lastName || mainlayout->count() < 1)
    {
        nameLabel->setText(name);
        lastName = name;

        if(name == Settings::getInstance().getUsername())
        {
            nameLabel->setForegroundRole(QPalette::Mid);
            nameLabel->setProperty("class", "msgUserName"); // for CSS styling
        }
        else
            nameLabel->setProperty("class", "msgFriendName"); // for CSS styling

        // Create line
        if(mainlayout->count() > 0)
        {
            QFrame *line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Plain);
            line->setForegroundRole(QPalette::Midlight);
            line->setProperty("class", "msgLine"); // for CSS styling
            mainlayout->addWidget(line);
        }
    }

    messageLabel->setText(smile(urlify(message)));
    qDebug() << messageLabel->text();
    timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));

    // Add row
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(nameLabel);
    hlayout->addWidget(messageLabel);
    hlayout->addWidget(timeLabel);
    mainlayout->addLayout(hlayout);
}

void MessageDisplayWidget2::appendErrorMessage(const QString &message)
{
    QPalette errorPal;
    errorPal.setColor(QPalette::Foreground, Qt::red);

    // Setup new row
    QLabel *nameLabel = new QLabel(this);
    nameLabel->setMaximumWidth(50);
    nameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    nameLabel->setPalette(errorPal);
    nameLabel->setProperty("class", "msgError"); // for CSS styling
    nameLabel->setAlignment(Qt::AlignLeading |
                            Qt::AlignLeft    |
                            Qt::AlignTop);

    QLabel *messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);
    messageLabel->setPalette(errorPal);
    messageLabel->setProperty("class", "msgError"); // for CSS styling
    messageLabel->setAlignment(Qt::AlignLeading |
                               Qt::AlignLeft    |
                               Qt::AlignTop);

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    timeLabel->setPalette(errorPal);
    timeLabel->setProperty("class", "msgError"); // for CSS styling
    timeLabel->setAlignment(Qt::AlignRight |
                            Qt::AlignTop   |
                            Qt::AlignTrailing);

    // Insert message
    if("*" != lastName || mainlayout->count() < 1)
    {
        nameLabel->setText("*");
        lastName = "*";

        // Create line
        if(mainlayout->count() > 0)
        {
            QFrame *line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Plain);
            line->setForegroundRole(QPalette::Midlight);
            line->setProperty("class", "msgLine"); // for CSS styling
            mainlayout->addWidget(line);
        }
    }

    messageLabel->setText(message);
    timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));

    // Add row
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(nameLabel);
    hlayout->addWidget(messageLabel);
    hlayout->addWidget(timeLabel);
    mainlayout->addLayout(hlayout);
}

void MessageDisplayWidget2::moveScrollBarToBottom(int min, int max)
{
    Q_UNUSED(min);
    this->verticalScrollBar()->setValue(max);
}

QString MessageDisplayWidget2::urlify(QString string)
{
    return string.replace(QRegularExpression("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QString MessageDisplayWidget2::smile(QString text)
{
    QHashIterator<QString, QStringList> i(smileyList);
    while(i.hasNext())
    {
        i.next();
        foreach (QString smileytext, i.value())
            text.replace(smileytext, QString("<img src=\"%1\" />").arg(i.key()));
    }
    return text;
}
