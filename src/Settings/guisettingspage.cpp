#include "guisettingspage.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>

GuiSettingsPage::GuiSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{

}

void GuiSettingsPage::buildGui()
{
    QVBoxLayout *layout        = new QVBoxLayout(this);

    // Message Style
    QGroupBox   *msgBox        = new QGroupBox(tr("Message style"), this);
    QFormLayout *msgFormLayout = new QFormLayout;
    QLabel      *msgLabel      = new QLabel(tr("Message style"), this);
    messageStyles              = new QComboBox(this);

    messageStyles->addItem(tr("default"), "default");
    messageStyles->addItem(tr("IRC"),     "irc");

    msgFormLayout->setWidget(0, QFormLayout::LabelRole, msgLabel);
    msgFormLayout->setWidget(0, QFormLayout::FieldRole, messageStyles);
    msgBox->setLayout(msgFormLayout);

    layout->addWidget(msgBox);
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();

    const QVariant style = settings.getMessageStyle();
    messageStyles->setCurrentIndex(messageStyles->findData(style));
}

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setMessageStyle(messageStyles->itemData(messageStyles->currentIndex()).toString());
}
