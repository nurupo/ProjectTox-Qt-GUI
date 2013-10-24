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

#include "editablelabelwidget.hpp"
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
    QString friendAddress;

    EditableLabelWidget* usernameWidget;
    EditableLabelWidget* statusMessageWidget;

    QToolButton* createToolButton(const QIcon& icon, const QSize iconSize, const QString& toolTip);

private slots:
    void onUsernameChanged(const QString& newUsername, const QString& oldUsername);
    void onStatusMessageChanged(const QString& newStatusMessage, const QString& oldStatusMessage);
    void onStatusActionTriggered();
    void onCopyFriendAddressButtonClicked();

public slots:
    void setFriendAddress(const QString &friendAddress);
    void setStatus(Status status);
    void setUsername(const QString& username);
    void setStatusMessage(const QString& statusMessage);

signals:
    void statusSelected(Status status);
    void usernameChanged(const QString& username);
    void statusMessageChanged(const QString& statusMessage);

};

#endif // OURUSERITEMWIDGET_HPP
