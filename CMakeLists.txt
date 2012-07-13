# See http://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
cmake_minimum_required(VERSION 2.6)

project(Octopus CXX)

add_subdirectory(qjson)
include_directories("qjson/src/")

if(CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(EnableCXX11)
include(EnableWarnings)

option(ENABLE_DOCUMENTATION "Build documentation")
if(ENABLE_DOCUMENTATION)
  add_subdirectory(doc)
endif()

check_CXX11()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAG}")
enable_Warnings()

find_package(EIToolkit REQUIRED)
include_directories(${EIToolkit_INCLUDE_DIR})

find_package(Qt4 REQUIRED)

add_definitions(-DQT_USE_QSTRINGBUILDER)

if(MSVC)
  set(Boost_USE_STATIC_LIBS ON)
endif()

set(BOOST_components system chrono)
find_package(Boost COMPONENTS ${BOOST_components} REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DBOOST_ALL_NO_LIB)

find_package(Sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIR})

set(Octopus_SOURCES
  src/common/plotsettings.cpp
  src/common/value.cpp
  src/datamodel/abstractdataseries.cpp
  src/datamodel/databaseadapter.cpp
  src/datamodel/dataprovider.cpp
  src/datamodel/doubleseries.cpp
  src/datamodel/sqlite.cpp
  src/datamodel/stringseries.cpp
  src/dialogs/plotsettingsdialog.cpp
  src/dialogs/sourcedialog.cpp
  src/export/CSVExporter.cpp
  src/export/exporterfactory.cpp
  src/export/exporthandler.cpp
  src/record/recorder.cpp
  src/main/main.cpp
  src/main/mainwindow.cpp
  src/main/startscreen.cpp
  src/main/viewmanager.cpp
  src/mainview/cursor.cpp
  src/mainview/presentationarea.cpp
  src/mainview/recordselection.cpp
  src/mainview/selection.cpp
  src/mainview/timeline.cpp
  src/mainview/timescrollbar.cpp
  src/mainview/tracks/discretegraph.cpp
  src/mainview/tracks/interpolatinggraph.cpp
  src/mainview/tracks/qcustomplot.cpp
  src/mainview/tracks/track.cpp
  src/network/networkadapter.cpp
  src/time/timemanager.cpp
)

set(Octopus_HEADERS
  include/common/common.h
  include/common/measure.h
  include/common/metadata.h
  include/common/plotsettings.h
  include/common/serializable.h
  include/common/value.h
  include/common/visitor.h
  include/datamodel/abstractdataseries.h
  include/datamodel/databaseadapter.h
  include/datamodel/dataprovider.h
  include/datamodel/DBUtil.h
  include/datamodel/doubleseries.h
  include/datamodel/sqlite.h
  include/datamodel/stringseries.h
  include/dialogs/plotsettingsdialog.h
  include/dialogs/sourcedialog.h
  include/export/exporthandler.h
  include/export/CSVExporter.h
  include/export/exporter.h
  include/export/exporterfactory.h
  include/main/mainwindow.h
  include/main/startscreen.h
  include/main/viewmanager.h
  include/mainview/cursor.h
  include/mainview/presentationarea.h
  include/mainview/recordselection.h
  include/mainview/selection.h
  include/mainview/timeline.h
  include/mainview/timescrollbar.h
  include/mainview/tracks/discretegraph.h
  include/mainview/tracks/graph.h
  include/mainview/tracks/interpolatinggraph.h
  include/mainview/tracks/qcustomplot.h
  include/mainview/tracks/track.h
  include/network/networkadapter.h
  include/record/recorder.h
  include/time/timemanager.h
)

set(Octopus_FORMS
  ui/mainwindow.ui
  ui/plotsettingsdialog.ui
  ui/sourcedialog.ui
  ui/startscreen.ui
  ui/track.ui
)
set(Octopus_RESOURCES
  rc/resources.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)

qt4_wrap_cpp(Octopus_HEADERS_MOC ${Octopus_HEADERS})
qt4_wrap_ui(Octopus_FORMS_HEADERS ${Octopus_FORMS})
qt4_add_resources(Octopus_RESOURCES_RCC ${Octopus_RESOURCES})
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(Octopus ${Octopus_SOURCES} ${Octopus_HEADERS}
    ${Octopus_HEADERS_MOC}
    ${Octopus_FORMS_HEADERS} 
    ${Octopus_RESOURCES_RCC})

set(Octopus_LIBRARIES EIToolkit qjson ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} ${QT_LIBRARIES})

target_link_libraries(Octopus ${Octopus_LIBRARIES})

if(FALSE)
add_subdirectory(contrib)
enable_testing()

# set(<BINARY_NAME> <TEST_FILE_PATHS>)
# Man kann mehrere Tests in ein Binary linken. Dazu einfach weitere .cpp-Files mit Leerzeichen anschließen.
set(SampleTest_Sources test/sample.cpp)

include_directories(${gtest_SOURCE_DIR}/include)

# add_executable(<BINARY_NAME> <FILES_NEEDED>)
# Benötigte Dateien mit Leerzeichen trennen.
add_executable(SampleTest ${SampleTest_Sources} ${Octopus_SOURCES} ${Octopus_HEADERS_MOC} ${Octopus_FORMS_HEADERS} ${Octopus_RESOURCES_RCC})

# target_link_libraries(<BINARY_NAME> ...)
# Jedes Binary muss separat mit den benötigten Libraries verlinkt werden.
target_link_libraries(SampleTest gtest gtest_main ${Octopus_LIBRARIES})

# add_test(<TEST_NAME> <BINARY>)
add_test(AllTestsInSampleTest SampleTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS SampleTest)
endif()
