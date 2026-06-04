cmake_minimum_required(VERSION 3.16)

project(Seteer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(TS_FILES Seteer_zh_CN.ts)

add_executable(Seteer
  main.cpp
  ${TS_FILES}
  game.h
  game.cpp
  Card.h
  carditem.h
  carditem.cpp
  carddata.h
  carddata.cpp
  resources.qrc
  stringspace.h
  stringspace.cpp
  effectvalue.h
  effectvalue.cpp
  condition.h
  condition.cpp
  effectaction.h
  effectaction.cpp
  effect.h
  entity.h entity.cpp
  BattleContext.h
  mainwindow.h
  mainwindow.cpp

  savedata.h
  savedata.cpp
  GameOverMenu.h
  GameOverMenu.cpp
  blessingdata.h
  blessingdata.cpp
  runmanager.h
  runmanager.cpp
  mainwindow.ui
  eventpage.h eventpage.cpp
  enemydata.h enemydata.cpp
  Status.h
  abilitydata.h abilitydata.cpp
)
target_link_libraries(Seteer PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Seteer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS Seteer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
