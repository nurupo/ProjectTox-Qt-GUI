# Qt GUI for Tox

[![Build Status](https://travis-ci.org/nurupo/ProjectTox-Qt-GUI.png?branch=master)](https://github.com/nurupo/ProjectTox-Qt-GUI)

A front end for [ProjectTox Core](https://github.com/irungentoo/ProjectTox-Core) written in Qt5 and C++.

Please note that developent is in very beginning and the project is not ready to use yet.

## Screenshot

![Hi There](https://raw.github.com/nurupo/ProjectTox-Qt-GUI/master/screenshot.png "Version 0.1.0 running on Windows 7")

## Contributing

* [Fork the repo](https://help.github.com/articles/fork-a-repo)

* Make your changes

* [Submit a pull request](https://help.github.com/articles/using-pull-requests)

## Compiling

### Linux/Unix with X11

To compile this project, you need to install Qt 5 development libraries.

Then run

_git clone https://github.com/nurupo/ProjectTox-Qt-GUI_

_cd ProjectTox-Qt-GUI_

this project is only a Qt GUI, and does not contain the core libraries. To install them, you need to run

_git submodules init && git submodules update_

now compile everything:

_cd projectfiles/QtCreator; qmake && make -j3_

You should get an executable called Tox-Qt-GUI. You can run it by typing _./TOX-Qt-GUI_ in the terminal.

(Note: you may also need to install NaCl encryption library for [ProjectTox Core](https://github.com/irungentoo/ProjectTox-Core).)

### OS X

Installation should be similar to other Unix systems, but it is not tested yet.

### Windows

## Contact

You can usually find me at Tox channel [irc://chat.freenode.net:6667/InsertProjectNameHere](irc://chat.freenode.net:6667/InsertProjectNameHere).

## License

Qt GUI for Tox is licensed under [GPLv3](COPYING).
