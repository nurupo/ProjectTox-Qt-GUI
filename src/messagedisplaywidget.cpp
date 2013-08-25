#include "messagedisplaywidget.hpp"

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
#include "messagelabel.hpp"
#include "emoticonmenu.hpp"

MessageDisplayWidget::MessageDisplayWidget(QWidget *parent) :
    QScrollArea(parent)
{
    // Scroll down on new Message
    QScrollBar* scrollbar = verticalScrollBar();
    connect(scrollbar, &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    QWidget *widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setWidget(widget);

    mainlayout = new QVBoxLayout(widget);
    mainlayout->setSpacing(1);
    mainlayout->setContentsMargins(1,1,1,1);
}

void MessageDisplayWidget::appendMessage(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId)
{
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom);
    QHBoxLayout *newLayout = createNewLine(name, message, messageId);
    mainlayout->addLayout(newLayout);
}

void MessageDisplayWidget::prependMessage(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId)
{
    disconnect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &MessageDisplayWidget::moveScrollBarToBottom);
    QHBoxLayout *newLayout = createNewLine(name, message, messageId);
    mainlayout->insertLayout(0, newLayout);
}

void MessageDisplayWidget::moveScrollBarToBottom(int min, int max)
{
    Q_UNUSED(min);
    this->verticalScrollBar()->setValue(max);
}

QString MessageDisplayWidget::urlify(QString string)
{
    return string.replace(QRegularExpression("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QHBoxLayout *MessageDisplayWidget::createNewLine(const QString &name, const QString &message/*, const QString &timestamp*/, int messageId)
{
    ElideLabel *nameLabel = new ElideLabel(this);
    nameLabel->setMaximumWidth(50);
    nameLabel->setTextElide(true);
    nameLabel->setTextElideMode(Qt::ElideRight);
    nameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    nameLabel->setToolTip(name);
    nameLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

    MessageLabel *messageLabel = new MessageLabel(this);
    messageLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    if(messageId) // Message added to send que
    {
        messageLabel->setMessageId(messageId);
        messageLabel->setProperty("class", "msgMessage"); // for CSS styling
        messageLabel->setText(EmoticonMenu::smile(urlify(message)));
    }
    else // Error
    {
        QPalette errorPal;
        errorPal.setColor(QPalette::Foreground, Qt::red);
        messageLabel->setPalette(errorPal);
        messageLabel->setProperty("class", "msgError"); // for CSS styling
        messageLabel->setText(urlify(message).prepend("<img src=\":/icons/error.png\" /> "));
        messageLabel->setToolTip(tr("Couldn't send the message!"));
    }

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    timeLabel->setForegroundRole(QPalette::Mid);
    timeLabel->setProperty("class", "msgTimestamp"); // for CSS styling
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignTop | Qt::AlignTrailing);
    timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));

    // Insert name if name has changed
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

    // Return new line
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(nameLabel);
    hlayout->addWidget(messageLabel);
    hlayout->addWidget(timeLabel);
    return hlayout;
}
