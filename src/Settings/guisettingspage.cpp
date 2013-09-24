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
    QVariant data(emoijPack.save());
    smileypackCombobox->addItem("☺ "+emoijPack.getName(), data);

    // Insert smileypacks
    searchSmileyPacks();

    // Load smileypack
    int index = smileypackCombobox->findData(settings.getSmileyPack());
    if (index < 0) {
        index = 0;
    }
    smileypackCombobox->setCurrentIndex(index);
}

void GuiSettingsPage::applyChanges()
{
    Settings& settings = Settings::getInstance();
    settings.setAnimationEnabled(enableAnimationCheckbox->isChecked());
    settings.setDejavuFont(useDejavuFontCheckbox->isChecked());
    settings.setSmileyPack(smileypackCombobox->itemData(smileypackCombobox->currentIndex()).toByteArray());
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
        if (!newPack.parseFile(f.absoluteFilePath())) {
            continue;
        }

        QVariant data(newPack.save());
        smileypackCombobox->addItem(QIcon(it.filePath()+QDir::separator()+newPack.getIcon()), newPack.getName(), data);
    }
}

void GuiSettingsPage::updateSmileypackDetails(int index)
{
    Smileypack pack(smileypackCombobox->itemData(index).toByteArray());

    QString version = pack.getVersion();
    if (!version.isEmpty()) {
        version.prepend(" v");
    }
    QString website = pack.getWebsite();
    if (!website.isEmpty()) {
        website = QString("<br><a href=\"%1\">%1</a>").arg(website);
    }

    QString desc = tr("<b>%1</b>%2 by %3<br>\"<i>%4</i>\"%5").arg(pack.getName(), version, pack.getAuthor(), pack.getDescription(), website);
    smileypackDescLabel->setText(desc);
}
