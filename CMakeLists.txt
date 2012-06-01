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
  include/mainwindow.h
  include/common.h
  include/abstractdataseries.h
  include/stringseries.h
  include/doubleseries.h
  include/dataprovider.h
  include/value.h
  include/gui/track.h
  include/gui/presentationarea.h
  include/gui/timeline.h
  include/gui/qcustomplot.h
  include/gui/mainview.h
  include/gui/trackscene.h
  include/gui/presentationitem.h
  include/gui/cursor.h
  include/gui/selection.h
)
SET(Octopus_FORMS
  ui/mainwindow.ui
  ui/track.ui
)
SET(Octopus_RESOURCES
  rc/resources.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
