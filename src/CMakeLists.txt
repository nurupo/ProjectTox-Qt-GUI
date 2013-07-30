set(TOX-Qt-GUI_SOURCES
  main.cpp
  mainwindow.cpp
  friendswidget.cpp
  addfrienddialog.cpp
  friendproxymodel.cpp
  filterwidget.cpp
  customhinttreeview.cpp
  chatpagewidget.cpp
  pageswidget.cpp
  inputtextwidget.cpp
  messagedisplaywidget.cpp
  frienditemwidget.cpp
  ouruseritemwidget.cpp
  renameeditwidget.cpp
  status.cpp
  friendrequestdialog.cpp
  customhinttextedit.cpp
  elidelabel.cpp
  core.cpp
  Settings/abstractsettingspage.cpp
  Settings/basicsettingsdialog.cpp
  Settings/settings.cpp
  dhteditdialog.cpp
  closeapplicationdialog.cpp
  starter.cpp
)

set(TOX-Core_SOURCES
  ../submodules/ProjectTox-Core/core/DHT.c
  ../submodules/ProjectTox-Core/core/Lossless_UDP.c
  ../submodules/ProjectTox-Core/core/Messenger.c
  ../submodules/ProjectTox-Core/core/net_crypto.c
  ../submodules/ProjectTox-Core/core/network.c
  ../submodules/ProjectTox-Core/core/friend_requests.c
  ../submodules/ProjectTox-Core/core/LAN_discovery.c
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/ProjectTox-Core/core/
  ${SODIUM_INCLUDE_DIR}
)

add_executable(TOX-Qt-GUI ${TOX-Qt-GUI_SOURCES} ${TOX-Qt-GUI_RCCS_SOURCES} ${TOX-Core_SOURCES})
qt5_use_modules(TOX-Qt-GUI Core Widgets Gui)
target_link_libraries(TOX-Qt-GUI ${SODIUM_LIBRARY})

file(COPY ../resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

