#include "emoticonmenu.h"

#include <QGridLayout>
#include <QToolButton>
#include <QDebug>
#include <QHash>

EmoticonMenu::EmoticonMenu(QWidget *parent) :
    QMenu(parent)
{
    layout = new QGridLayout(this);
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(0);

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

    /// \todo Recalculate menu size. It doesn't fit to screen size.
}

EmoticonMenu::SmileyList EmoticonMenu::getSmileyList()
{
    return smileyList;
}

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
    smileyList.insert(imgPath, texts);
}

void EmoticonMenu::onEmoticonTriggered()
{
    emit insertEmoticon(QObject::sender()->property("smiley").toString());
}
