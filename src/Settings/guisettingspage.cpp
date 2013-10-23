#include "guisettingspage.hpp"

#include "settings.hpp"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

GuiSettingsPage::GuiSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void GuiSettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QGroupBox *animationGroup = buildAnimationGroup();

    layout->addWidget(animationGroup);
    layout->addStretch(0);
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    enableAnimationCheckbox->setChecked(settings.isAnimationEnabled());
}

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setAnimationEnabled(enableAnimationCheckbox->isChecked());
}

QGroupBox *GuiSettingsPage::buildAnimationGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smoth animation"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    enableAnimationCheckbox = new QCheckBox(tr("Enable animation"), group);

    layout->addWidget(enableAnimationCheckbox);
    return group;
}
