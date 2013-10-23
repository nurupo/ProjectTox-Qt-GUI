/*
    Copyright (C) 2013 by Martin Kröll <technikschlumpf@web.de>

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

#ifndef FRIENDREQUESTLABEL_H
#define FRIENDREQUESTLABEL_H

#include <QWidget>

class QFrame;
class QLabel;
class ElideLabel;
class QPushButton;
class QToolButton;
class QPropertyAnimation;

/*! FriendRequestWidget shows incoming friend requests.
 *
 * CSS classes for styling
 * -----------------------
 *
 * Object         | Qt class           | CSS class
 * ---------------|--------------------|--------------
 * Button         | QPushButton        | frqButton
 * Details widget | QFrame             | frqDetails
 */
class FriendRequestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FriendRequestWidget(QWidget *parent = 0);
    
signals:
    void userAccepted(QString);
    
public slots:
    void addFriendRequest(const QString &userId, const QString &msg);

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void acceptUser();
    void rejectUser();
    void showDetails(bool show);
    void nextRequest();

private:
    QPushButton *button;
    QFrame      *detailsWidget;
    ElideLabel  *clientId;
    QLabel      *message;
    QToolButton *acceptButton;
    QToolButton *rejectButton;
    QToolButton *nextButton;

    QPropertyAnimation *animation;

    QHash<QString, QString>                requests;
    QMutableHashIterator<QString, QString> it;

    void updateGui();
    void goFirst();
};

#endif // FRIENDREQUESTLABEL_H
