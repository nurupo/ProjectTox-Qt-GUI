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

#ifndef OURUSERITEMWIDGET_HPP
#define OURUSERITEMWIDGET_HPP

#include "imagecombobox.hpp"
#include "renameeditwidget.hpp"
#include "status.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QToolButton>
#include <QWidget>

class OurUserItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OurUserItemWidget(QWidget* parent = 0);

private:
    QToolButton* statusButton;
    QLabel* usernameLabel;
    RenameEditWidget* usernameEdit;
    QStackedWidget* usernameStackedWidget;
    QString userId;

    QToolButton* createToolButton(const QIcon& icon, const QSize iconSize, const QString& toolTip);

private slots:
    void onUsernameChange();
    void onRenameUsernameButtonClicked();
    void onStatusActionTriggered();
    void onCopyUserIdButtonClicked();

public slots:
    void setUserId(const QString &userId);
    void setStatus(Status status);
    void setUsername(const QString& username);

signals:
    void statusSelected(Status status);
    void usernameChanged(const QString& username);

};

#endif // OURUSERITEMWIDGET_HPP
