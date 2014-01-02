##Installation

###Linux:

This project depends on Qt5.2.0 and [ProjectTox-Core](http://github.com/irungentoo/ProjectTox-Core), which in turn depends on [libsodium](http://github.com/jedisct1/libsodium).

You should get and install **Qt5.2.0 or higher**.
You can get Qt5 precompiled binaries and source code to compile them yourself from [qt-project website](http://qt-project.org/downloads).

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
git clone --recursive https://github.com/nurupo/ProjectTox-Qt-GUI.git
cd ProjectTox-Qt-GUI
mkdir build && cd build
qmake -Wall ../projectfiles/QtCreator/TOX-Qt-GUI.pro
make
```
On success, this will build a `TOX-Qt-GUI` executable.

Note the use of `--recursive` in `git clone`. This will automatically initialize and update all submodules.

Note that you should not update submodules on your own if you don't know what you are doing. Newer versions of submodules might not be compatible with the project (as in "won't work" or even "won't build"), and may require testing and some code changes.

Note that `qmake` might not be found by bash or it might be found, but it's from Qt4 installation. In the case of qmake not working, make sure you use the right qmake.
If you have installed Qt5 from the precompiled binaries provided on qt-project website, you should use qmake from Qt5's installation directory.
For example, my qmake was located at `/home/nurupo/Qt5.2.0/5.2.0/gcc_64/bin/qmake`.

If Qt complains about missing GL library, install `libgl-dev` package. The exact package name might differ from distribution to distribution, but you get the idea.

Also, using `make` with `-jN` parameter, where `N` is a number of cores your processor has + 1, should decrease build time greatly.
For example, if your machine is dual core, use `make -j3`.

If you want to build the GUI by using Qt Creator, rather than terminal commands, follow the Windows guide starting at "Then open the Qt Creator...".

###Windows:

You should get and install **MinGW version** of **Qt5.2.0 or higher** from [qt-project website](http://qt-project.org/downloads).

If you don't already have a git shell installed, you can get one [here](http://git-scm.com/download/win).

Open the git shell, navigate to a desired directory and run:
```cmd
git clone --recursive https://github.com/nurupo/ProjectTox-Qt-GUI.git
cd ProjectTox-Qt-GUI
```

Note the use of `--recursive` in `git clone`. This will automatically initialize and update all submodules.

Note that you should not update submodules on your own if you don't know what you are doing. Newer versions of submodules might not be compatible with the project (as in "won't work" or even "won't build"), and may require testing and some code changes.

Next you will need to create `libs` directory in repo's root, then download [a MinGW release of libsodium 0.4.2+](https://download.libsodium.org/libsodium/releases/), extract `libsodium-win32` into previously created `libs`directory and rename `libsodium-win32` into `sodium`. After all those manipulations `ProjectTox-Qt-GUI\libs\sodium\include` should become a valid path.
Alternatively, instead of downloading the precompiled binaries of libsodium, you can build the library from the [libsodium](https://github.com/jedisct1/libsodium) repo yourself and repeat the steps above using it. 0.4.2+ versions should work.

Then open the Qt Creator, which should have been installed with Qt, select `File -> Open File or Project...` and open the project file at `ProjectTox-Qt-GUI\projectfiles\QtCreator\TOX-Qt-GUI.pro`, select Qt5.2.0+ libraries and click on `Configure Project`.

After that find `Debug` on **bottom** left and switch it to `Release`.

If you want to speed up the building process a little, click on `Projects` on the left, under `Build Steps` find `Make`, click on its `Details` and add `-jN` to the `Make arguments`, where `N` is a number of cores your processor has + 1. For example, if your machine is dual core, add `-j3`.

Close the `Details` and hit the big green arrow on bottom left (or press Ctlr+R), that should build and run the GUI in Release mode.

You might also need to install [Windows SDK for your version of Windows OS](https://en.wikipedia.org/wiki/Microsoft_Windows_SDK#Versions).
