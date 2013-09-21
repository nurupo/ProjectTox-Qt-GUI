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

#include "smileypackparser.h"

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

        qDebug() << it.filePath()+QDir::separator()+"theme";

        SmileypackParser parser;
        parser.parseFile(it.filePath()+QDir::separator()+"theme");

        QString icon = it.filePath()+QDir::separator()+parser.getHeader().value("Icon");
        QVariant data;
        data.setValue(parser.getHeader());

        smileypackCombobox->addItem(QIcon(icon), parser.getHeader().value("Name"), data);
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
    smileypackWebLabel  = new QLabel(group);

    smileypackNameLabel->setWordWrap(true);
    smileypackNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    smileypackDescLabel->setWordWrap(true);
    smileypackDescLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    smileypackWebLabel->setVisible(false);
    smileypackWebLabel->setOpenExternalLinks(true);

    layout->addWidget(smileypackCombobox);
    layout->addWidget(smileypackNameLabel);
    layout->addWidget(smileypackDescLabel);
    layout->addWidget(smileypackWebLabel);
    return group;
}

void GuiSettingsPage::updateSmileypackDetails(int index)
{
    SmileypackParser::HeaderHash header = smileypackCombobox->itemData(index).value<SmileypackParser::HeaderHash>();

    QString version;
    if (!header.value("Version").isEmpty()) {
        version = "v" + header.value("Version");
    }

    smileypackNameLabel->setText(tr("<b>%1</b> %2 by %3").arg(header.value("Name"), version, header.value("Author")));
    smileypackDescLabel->setText(QString("\"<i>%1</i>\"").arg(header.value("Description")));

    if (!header.value("Website").isEmpty()) {
        smileypackWebLabel->setText(QString("<a href=\"%1\">%1</a>").arg(header.value("Website")));
        smileypackWebLabel->setVisible(true);
    }
    else {
        smileypackWebLabel->clear();
        smileypackWebLabel->setVisible(false);
    }
}
