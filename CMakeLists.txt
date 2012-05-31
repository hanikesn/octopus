# See http://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
cmake_minimum_required(VERSION 2.6)
PROJECT(Octopus)
FIND_PACKAGE(Qt4 REQUIRED)

SET(Octopus_SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/abstractdataseries.cpp
  src/stringseries.cpp
  src/doubleseries.cpp
  src/dataprovider.cpp
  src/value.cpp
  src/gui/track.cpp
  src/gui/presentationarea.cpp
  src/gui/timeline.cpp
  src/gui/qcustomplot.cpp
  src/gui/mainview.cpp
  src/gui/trackscene.cpp
  src/gui/presentationitem.cpp
  src/gui/cursor.cpp
  src/gui/selection.cpp
)
SET(Octopus_HEADERS
  src/mainwindow.h
  src/common.h
  src/abstractdataseries.h
  src/stringseries.h
  src/doubleseries.h
  src/dataprovider.h
  src/value.h
  src/gui/track.h
  src/gui/presentationarea.h
  src/gui/timeline.h
  src/gui/qcustomplot.h
  src/gui/mainview.h
  src/gui/trackscene.h
  src/gui/presentationitem.h
  src/gui/cursor.h
  src/gui/selection.h
)
SET(Octopus_FORMS
  ui/mainwindow.ui
  ui/track.ui
)
SET(Octopus_RESOURCES
  rc/resources.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

QT4_WRAP_CPP(Octopus_HEADERS_MOC ${Octopus_HEADERS})
QT4_WRAP_UI(Octopus_FORMS_HEADERS ${Octopus_FORMS})
QT4_ADD_RESOURCES(Octopus_RESOURCES_RCC ${Octopus_RESOURCES})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(Octopus ${Octopus_SOURCES} 
    ${Octopus_HEADERS_MOC}
    ${Octopus_FORMS_HEADERS} 
    ${Octopus_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(Octopus ${QT_LIBRARIES})
