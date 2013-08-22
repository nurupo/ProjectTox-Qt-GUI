#include "emoticonmenu.hpp"

#include <QGridLayout>
#include <QToolButton>
#include <QDebug>
#include <QHash>
#include <QWidgetAction>
#include <QRegularExpression>
#include <QTextDocument>

EmoticonMenu::EmoticonMenu(QWidget *parent) :
    QMenu(parent)
{
    layout = new QGridLayout(this);
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(0);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(new QWidget(this));
    action->defaultWidget()->setLayout(layout);
    this->addAction(action);

    // some Skype smileys
    addEmoticon(":/icons/emoticons/emotion_smile.png",    {":)", ":-)", ":o)"});
    addEmoticon(":/icons/emoticons/emotion_sad.png",      {":(", ":-("});
    addEmoticon(":/icons/emoticons/emotion_grin.png",     {":D", ":-D"});
    addEmoticon(":/icons/emoticons/emotion_cool.png",     {"8)", "8-)"});
    addEmoticon(":/icons/emoticons/emotion_suprised.png", {":O", ":-O"});
    addEmoticon(":/icons/emoticons/emotion_wink.png",     {";)", ";-)"});
    addEmoticon(":/icons/emoticons/emotion_cry.png",      {";(", ";-("});
    addEmoticon(":/icons/emoticons/emotion_sweat.png",    {"(:|"});
    // ...
    addEmoticon(":/icons/emoticons/emotion_kiss.png",     {":*", ":-*"});
    addEmoticon(":/icons/emoticons/emotion_tongue.png",   {":P", ":-P"});
    // ...
    addEmoticon(":/icons/emoticons/emotion_doubt.png",    {":^)", ":^-)"});
    // ...
    addEmoticon(":/icons/emoticons/emotion_love.png",     {"(inlove)"});
    addEmoticon(":/icons/emoticons/emotion_evilgrin.png", {"]:)", "]:-)"});
    // ...
    addEmoticon(":/icons/emoticons/emotion_angel.png",    {"O:)", "O:-)", "o:)", "o:-)", "(angel)"});
    // ...
}

/*! Get a hash of all smileys. That can be used for functions like desmile. */
EmoticonMenu::SmileyHash &EmoticonMenu::getSmileyHash()
{
    static SmileyHash smileyHash;
    return smileyHash;
}

QString EmoticonMenu::smile(QString text)
{
    QHashIterator<QString, QStringList> i(getSmileyHash());
    while(i.hasNext())
    {
        i.next();
        foreach (QString smileytext, i.value())
            text.replace(smileytext, QString("<img src=\"%1\" />").arg(i.key()));
    }
    return text;
}

QString EmoticonMenu::desmile(QString htmlText)
{
    // Replace smileys by their textual representation
    int i=0;
    QRegularExpression re("<img[\\s]+[^>]*?((alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?)|(src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?))((src*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|(alt*?[\\s]?=[\\s\\\"\\']+(.*?)[\\\"\\']+.*?>)|>)");
    QRegularExpressionMatch match = re.match(htmlText, i);
    while(match.hasMatch())
    {
        // Replace smiley and match next
        htmlText.replace(match.captured(0), getSmileyHash().value(match.captured(5)).first());
        match = re.match(htmlText, ++i);
    }

    // convert to plain text
    QTextDocument doc;
    doc.setHtml(htmlText);
    return doc.toPlainText();
}

/*! Add smileys by setting a imgage file and a textlist of textual representations. */
void EmoticonMenu::addEmoticon(const QString &imgPath, const QStringList &texts)
{
    QToolButton *button = new QToolButton(this);
    button->setIcon(QIcon(imgPath));
    button->setAutoRaise(true);
    button->setToolTip(texts.first());
    button->setProperty("smiley", QString("<img src=\"%1\" />").arg(imgPath));
    connect(button, &QToolButton::clicked, this, &EmoticonMenu::onEmoticonTriggered);
    connect(button, &QToolButton::clicked, this, &EmoticonMenu::close);

    layout->addWidget(button, layout->count()/5, layout->count()%5);
    getSmileyHash().insert(imgPath, texts);
}

/*! Signal sends the (first) textual form of the clicked smiley. */
void EmoticonMenu::onEmoticonTriggered()
{
    emit insertEmoticon(QObject::sender()->property("smiley").toString());
}
