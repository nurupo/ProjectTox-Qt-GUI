#include "guisettingspage.h"

#include "settings.hpp"
#include "appinfo.hpp"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QRegularExpression>
#include <QComboBox>
#include <QLabel>

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

    // Look for smiley packs
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+QDir::separator()+AppInfo::name+QDir::separator()+"smileys");
    if(!dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+QDir::separator()+AppInfo::name+QDir::separator()+"smileys"))
        qDebug() << tr("ERROR Couldn't create smileypack directory.");
    //qDebug() << dir.absolutePath();

    // Go to all packs
    dir.setFilter(QDir::Dirs|QDir::NoDot|QDir::NoDotDot);
    QDirIterator it(dir);
    while (it.hasNext()) {
        it.next();

        QFileInfo f(it.filePath()+QDir::separator()+"theme");
        if(!f.exists())
            continue;

        QSettings packFile(f.absoluteFilePath(), QSettings::IniFormat);
        packFile.setIniCodec("UTF-8");

        QString name = packFile.value("head/Name").toString();
        QString icon = it.filePath()+QDir::separator()+packFile.value("head/Icon").toString();
        QStringList data;
        data << name << packFile.value("head/Author").toString() << packFile.value("head/Description").toString();
        smileypackCombobox->addItem(QIcon(icon), name, data);

        qDebug() << packFile.value("head/Author").toString();

        packFile.beginGroup("smileys");
        foreach (QString key, packFile.allKeys()) {
            //qDebug() << packFile.value(key).toString().split(QRegularExpression("\\s+"),QString::SkipEmptyParts);
        }
        packFile.endGroup();
    }
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

QGroupBox *GuiSettingsPage::buildSmileypackGroup()
{
    QGroupBox *group = new QGroupBox(tr("Smileypack"), this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    smileypackCombobox = new QComboBox(group);
    connect(smileypackCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSmileypackDetails(int)));

    smileypackNameLabel = new QLabel(group);
    smileypackDescLabel = new QLabel(group);
    smileypackDescLabel->setWordWrap(true);

    layout->addWidget(smileypackCombobox);
    layout->addWidget(smileypackNameLabel);
    layout->addWidget(smileypackDescLabel);
    return group;
}

void GuiSettingsPage::updateSmileypackDetails(int index)
{
    smileypackNameLabel->setText(tr("<b>%1</b> by %2").arg(smileypackCombobox->itemData(index).toStringList().at(0)).arg(smileypackCombobox->itemData(index).toStringList().at(1)));
    smileypackDescLabel->setText(smileypackCombobox->itemData(index).toStringList().at(2));
}
