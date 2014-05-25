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

#ifndef MESSAGEFILTER_HPP
#define MESSAGEFILTER_HPP

#include <QSortFilterProxyModel>
#include "message.hpp"

class MessageFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MessageFilter(QObject *parent = 0);

signals:

public slots:
    inline void filterPlain(bool hide)     { filterMessageType(hide, Message::Plain); }
    inline void filterAction(bool hide)    { filterMessageType(hide, Message::Action); }
    inline void filterNick(bool hide)      { filterMessageType(hide, Message::Nick); }
    inline void filterError(bool hide)     { filterMessageType(hide, Message::Error); }
    inline void filterDaychange(bool hide) { filterMessageType(hide, Message::DayChange); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    int mTypesToHide;

    void filterMessageType(bool hide, Message::Type type);
};

#endif // MESSAGEFILTER_HPP
