/*
    Copyright (C) 2014 by Martin Kröll <technikschlumpf@web.de>

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

#include "messagefilter.hpp"
#include "messagemodel.hpp"
#include <QDebug>

MessageFilter::MessageFilter(QObject *parent) :
    QSortFilterProxyModel(parent),
    mTypesToHide(0)
{
}

bool MessageFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    QModelIndex index = sourceModel()->index(sourceRow, 1);
    Message::Type messageType = (Message::Type) index.data(MessageModel::TypeRole).toInt();

    return (messageType & mTypesToHide) ? false : true;
}

void MessageFilter::filterMessageType(bool hide, Message::Type type)
{
    int old = mTypesToHide;
    mTypesToHide = (hide) ? old | type : old & ~type;

    if(old != mTypesToHide)
        invalidateFilter();
}
