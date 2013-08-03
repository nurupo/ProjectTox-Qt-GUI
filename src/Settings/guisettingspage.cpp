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
    QFormLayout *formLayout = new QFormLayout(this);
    QLabel *langLabel       = new QLabel(tr("GUI language"), this);
    langCombo               = new QComboBox(this);

    langCombo->addItem(QIcon(":/icons/flag_great_britain.png"), "English", QVariant("en_GB"));
    langCombo->addItem(QIcon(":/icons/flag_germany.png"), "German", QVariant("de_DE"));

    formLayout->setWidget(0, QFormLayout::LabelRole, langLabel);
    formLayout->setWidget(0, QFormLayout::FieldRole, langCombo);
    langBox->setLayout(formLayout);
    layout->addWidget(langBox);
    this->setLayout(layout);
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    const QVariant language = settings.getGuiLanguage();
    langCombo->setCurrentIndex(langCombo->findData(language));
}
