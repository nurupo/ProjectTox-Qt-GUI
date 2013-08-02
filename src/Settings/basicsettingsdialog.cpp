/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>

    This file is part of Tox Qt GUI.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the COPYING file for more details.
*/

#include "basicsettingsdialog.hpp"
#include "customhintlistwidget.hpp"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSplitter>

BasicSettingsDialog::BasicSettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QGridLayout* dialogLayout = new QGridLayout(this);

    listWidget = new CustomHintListWidget(this, QSize(10, 10));
    listWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    listWidget->setIconSize(QSize(24, 24));

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setCurrentIndex(0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(listWidget);
    splitter->addWidget(stackedWidget);

    dialogLayout->addWidget(splitter,   0, 0, 1, 1);
    dialogLayout->addWidget(buttonBox,  1, 0, 1, 1);

    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}

BasicSettingsDialog::~BasicSettingsDialog()
{
}

void BasicSettingsDialog::addPage(const QString& iconPath, const QString& name, AbstractSettingsPage* page)
{
    listWidget->addItem(new QListWidgetItem(QIcon(iconPath), name, listWidget));
    page->buildGui();
    page->setGui();
    stackedWidget->addWidget(page);
}

void BasicSettingsDialog::accept()
{
    for (int i = 0; i < stackedWidget->count(); i ++) {
        AbstractSettingsPage* settinsPage = static_cast<AbstractSettingsPage*>(stackedWidget->widget(i));
        settinsPage->applyChanges();
    }

    QDialog::accept();
}
