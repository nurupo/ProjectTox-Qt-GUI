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

#include "status.hpp"

const QList<StatusHelper::Info> StatusHelper::info =
{
    {"Online", ":/icons/status/online_small.png", ":/icons/status/online_small_active.png"},
    {"Away", ":/icons/status/away_small.png", ":/icons/status/away_small_active.png"},
    {"Busy", ":/icons/status/busy_small.png", ":/icons/status/busy_small_active.png"},
    {"Offline", ":/icons/status/offline_small.png", ":/icons/status/offline_small_active.png"}
};

StatusHelper::Info StatusHelper::getInfo(int status)
{
    return info.at(status);
}

StatusHelper::Info StatusHelper::getInfo(Status status)
{
    return StatusHelper::getInfo(static_cast<int>(status));
}
