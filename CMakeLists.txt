# See http://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
cmake_minimum_required(VERSION 2.6)
PROJECT(Octopus)
FIND_PACKAGE(Qt4 REQUIRED)

SET(Octopus_SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/gui/exampletrack.cpp
  src/gui/presentationarea.cpp
  src/gui/timeline.cpp
)
SET(Octopus_HEADERS
  src/mainwindow.h
  src/gui/exampletrack.h
  src/gui/presentationarea.h
  src/gui/timeline.h
)
SET(Octopus_FORMS
  ui/mainwindow.ui
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
