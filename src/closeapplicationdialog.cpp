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

#include "closeapplicationdialog.hpp"
#include "Settings/settings.hpp"
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>

CloseApplicationDialog::CloseApplicationDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Closing the application");

    QLabel* closeLabel = new QLabel("Do you really want to close the application?");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No, Qt::Horizontal, this);
    buttonBox->button(QDialogButtonBox::No)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CloseApplicationDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CloseApplicationDialog::reject);

    askAgainBox = new QCheckBox("Don't ask again",this);
    askAgainBox->setChecked(false);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(askAgainBox);
    buttonsLayout->addWidget(buttonBox);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(closeLabel,0,Qt::AlignHCenter);
    layout->addSpacing(8);
    layout->addLayout(buttonsLayout);
}

void CloseApplicationDialog::accept()
{
    Settings::getInstance().setShowConfrimationDialogOnClose(!askAgainBox->isChecked());
    qApp->quit();
    QDialog::accept();
}
