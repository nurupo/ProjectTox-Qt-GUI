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

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setGUILanguage(langCombo->itemData(langCombo->currentIndex()));


}

void GuiSettingsPage::buildGui()
{
    // Language
    QVBoxLayout *layout     = new QVBoxLayout(this);
    QGroupBox *langBox      = new QGroupBox(tr("Language"), this);
    QFormLayout *formLayout = new QFormLayout;
    QLabel *langLabel       = new QLabel(tr("GUI language"), this);
    langCombo               = new QComboBox(this);

    langCombo->addItem(QIcon(":/icons/flag_great_britain.png"), "English", QVariant("en_GB"));
    langCombo->addItem(QIcon(":/icons/flag_germany.png"), "Deutsch", QVariant("de_DE"));

    formLayout->setWidget(0, QFormLayout::LabelRole, langLabel);
    formLayout->setWidget(0, QFormLayout::FieldRole, langCombo);
    langBox->setLayout(formLayout);

    restartLabel = new QLabel(tr("You need to restart TOX."), this);
    restartLabel->setHidden(true);
    restartLabel->setDisabled(true);

    layout->addWidget(langBox);
    layout->addWidget(restartLabel);
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    const QVariant language = settings.getGuiLanguage();
    langCombo->setCurrentIndex(langCombo->findData(language));

    connect(langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(showRestartInfo(int)));
    //connect(langCombo, &QComboBox::currentIndexChanged, this, &GuiSettingsPage::showRestartInfo);
}

void GuiSettingsPage::showRestartInfo(int i)
{
    Q_UNUSED(i)
    Settings& settings = Settings::getInstance();
    if(langCombo->itemData(langCombo->currentIndex()) == settings.getGuiLanguage())
        restartLabel->hide();
    else
        restartLabel->show();
}
