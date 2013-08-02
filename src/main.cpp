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

//#define USEQML
#include <QApplication>
#ifdef USEQML
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "coreqml.hpp"
#else
#include "starter.hpp"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef USEQML
    CoreQml coreQml;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("coreQml", &coreQml);
    engine.load(QUrl("qrc:/qml/window/main.qml"));

#else
    Starter s;
#endif
    return a.exec();
}
