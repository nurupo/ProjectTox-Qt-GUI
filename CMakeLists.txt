project(TOX-Qt-GUI)

cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Sodium REQUIRED)

set(TOX-Qt-GUI_RCCS "resources/icons.qrc")

qt5_add_resources(TOX-Qt-GUI_RCCS_SOURCES ${TOX-Qt-GUI_RCCS})

add_subdirectory(src)