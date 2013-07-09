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

#include "frienditemdelegate.hpp"

#include <QApplication>
#include <QDebug>
#include <QPainter>

FriendItemDelegate::FriendItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void FriendItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLabel username(index.data(Qt::DisplayRole).toString());
    //username.setAlignment(Qt::AlignLeft);
    qDebug() << "called";
    username.render(painter);
    QStyledItemDelegate::paint(painter, option, index);
    //very useful http://qt-articles.blogspot.com/2010/07/how-to-customize-listview-in-qt-using.html
    //might be useful too http://doc.qt.digia.com/qq/qq24-delegates.html
}

