#include "guisettingspage.h"

#include "settings.hpp"
#include "appinfo.hpp"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QRegularExpression>
#include <QComboBox>
#include <QLabel>

#include "smileypack.h"

GuiSettingsPage::GuiSettingsPage(QWidget *parent) :
    AbstractSettingsPage(parent)
{
}

void GuiSettingsPage::buildGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(buildAnimationGroup());
    layout->addWidget(buildSmileypackGroup());
    layout->addStretch(0);
}

void GuiSettingsPage::setGui()
{
    const Settings& settings = Settings::getInstance();
    enableAnimationCheckbox->setChecked(settings.isAnimationEnabled());
    useDejavuFontCheckbox->setChecked(settings.isDejavuFont());

    // Insert Emoij
    Smileypack emoijPack;
    smileypackCombobox->addItem(emoijPack.getName(), tr("<b>%1</b> by %2<br>\"<i>%3</i>\"").arg(emoijPack.getName(), emoijPack.getAuthor(), emoijPack.getDescription()));

    // Insert smileypacks
    searchSmileyPacks();

    // Load smileypack
    smileypackCombobox->setCurrentText(settings.getSmileyPack());
}

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setAnimationEnabled(enableAnimationCheckbox->isChecked());
    settings.setDejavuFont(useDejavuFontCheckbox->isChecked());

    // Parse the selected smileypack
    Smileypack newPack;
    newPack.parseFile(Smileypack::packDir()+QDir::separator()+smileypackCombobox->currentText()+QDir::separator()+"theme");
    Smileypack::currentPack() = newPack;

    if(smileypackCombobox->currentText() != "Emoij") {
        settings.setSmileyPack(smileypackCombobox->currentText());
    }
    else {
        settings.setSmileyPack("");
    }
}

QGroupBox *GuiSettingsPage::buildAnimationGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smoth animation"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    enableAnimationCheckbox = new QCheckBox(tr("Enable animation"), group);

    layout->addWidget(enableAnimationCheckbox);
    return group;
}

QGroupBox *GuiSettingsPage::buildSmileypackGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smileypack"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    smileypackCombobox = new QComboBox(group);
    connect(smileypackCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSmileypackDetails(int)));

    smileypackDescLabel = new QLabel(group);
    smileypackDescLabel->setWordWrap(true);
    smileypackDescLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    smileypackDescLabel->setOpenExternalLinks(true);

    useDejavuFontCheckbox = new QCheckBox(group);
    useDejavuFontCheckbox->setText(tr("Use \"DejaVu Sans\" font."));
    useDejavuFontCheckbox->setToolTip(tr("This font supports Unicode 6.1 and Emoij."));

    layout->addWidget(smileypackCombobox);
    layout->addWidget(smileypackDescLabel);
    layout->addWidget(useDejavuFontCheckbox);
    return group;
}

void GuiSettingsPage::searchSmileyPacks()
{
    // go to smiley pack folder
    QDir dir(Smileypack::packDir());
    if(!dir.mkpath(Smileypack::packDir()))
        return;

    // Go through all packs
    dir.setFilter(QDir::Dirs|QDir::NoDot|QDir::NoDotDot);
    QDirIterator it(dir);
    while (it.hasNext()) {
        it.next();

        QFileInfo f(it.filePath()+QDir::separator()+"theme");
        if (!f.exists()) {
            continue;
        }

        Smileypack newPack;
        if (!newPack.parseFile(it.filePath()+QDir::separator()+"theme")) {
            continue;
        }

        QString version = newPack.getVersion();
        if (!version.isEmpty()) {
            version.prepend(" v");
        }
        QString desc = tr("<b>%1</b>%2 by %3<br>\"<i>%4</i>\"<br><a href=\"%5\">%5</a>").arg(newPack.getName(), version, newPack.getAuthor(), newPack.getDescription(), newPack.getWebsite());

        smileypackCombobox->addItem(QIcon(it.filePath()+QDir::separator()+newPack.getIcon()), f.dir().dirName(), desc);
    }
}

void GuiSettingsPage::updateSmileypackDetails(int index)
{
    smileypackDescLabel->setText(smileypackCombobox->itemData(index).toString());
}
