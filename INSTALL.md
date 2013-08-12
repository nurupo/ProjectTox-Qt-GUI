##Installation

###Linux:

This project depends on Qt5 and [ProjectTox-Core](http://github.com/irungentoo/ProjectTox-Core), which in turn depends on [libsodium](http://github.com/jedisct1/libsodium).

You should get and install Qt5.
You can get Qt5 precompiled binaries and source code to compile them yourself from [here](http://qt-project.org/downloads).

You should get and install [libsodium](https://github.com/jedisct1/libsodium):

(requires `automake` and `libtool` being installed)
```bash
git clone git://github.com/jedisct1/libsodium.git
cd libsodium
git checkout tags/0.4.2
./autogen.sh
./configure && make check
sudo make install
sudo ldconfig
```

Then get this repo and build the GUI:
```bash
git clone --recursive git://github.com/nurupo/ProjectTox-Qt-GUI.git
cd ProjectTox-Qt-GUI
mkdir build && cd build
qmake -Wall ../projectfiles/QtCreator/TOX-Qt-GUI.pro
make
```
On success, this will build a `TOX-Qt-GUI` executable.

Note that `qmake` might not be found by bash, in that case you should use qmake from Qt's installation directory.
For example, my qmake was located at `/home/nurupo/Qt5.1.0/5.0.1/gcc/bin/qmake`.

If Qt complains about missing GL library, install `libgl-dev` package.

Also, using `make` with `-jN` parameter, where `N` is a number of cores your processor has + 1, should decrease build time greatly.
For example, if your machine is dual core, use `make -j3`.

If you want to build GUI by using Qt Creator, rather than terminal commands, follow Windows guide starting at "Then open the Qt Creator...".

###Windows:

You should get and install **MinGW version** of Qt5 from [here](http://qt-project.org/downloads).

If you don't already have a git shell installed, you can use one that comes with [GitHub for Windows](http://windows.github.com/).

Open the git shell and run:
```cmd
git clone --recursive git://github.com/nurupo/ProjectTox-Qt-GUI.git
cd ProjectTox-Qt-GUI
```

Then download and extract [this archive containing precompiled binaries of libsodium 0.4.2 for Windows](https://download.libsodium.org/libsodium/releases/libsodium-win32-0.4.2.tar.gz) at `ProjectTox-Qt-GUI\libs\`, so that `TOX-Qt-GUI\libs\sodium\include` becomes a valid path. Note that there is no `libs` folder, you should create one.

Then open the Qt Creator, which should have been installed with Qt, select `File -> Open File or Project...` and open the project file at `ProjectTox-Qt-GUI\projectfiles\QtCreator\TOX-Qt-GUI.pro`, click on `Configure Project`.
After that find `Debug` on **bottom** left and switch it to `Release`.
Then click on `Projects` on the left, under `Build Steps` find `Make`, click on its `Details` and add `-jN` to the `Make arguments`, where `N` is number of cores your processor has + 1. For example, if your machine is dual core, add `-j3`.
Close the `Details` and hit the big green arrow on bottom left (or press Ctlr+R), that should build and run the GUI in Release mode.

You might also need to install [Windows SDK for your version of Windows OS](https://en.wikipedia.org/wiki/Microsoft_Windows_SDK#Versions).
