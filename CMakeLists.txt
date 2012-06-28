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

check_CXX11()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAG}")
add_subdirectory(contrib)
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
  src/abstractdataseries.cpp
  src/CVSExporter.cpp
  src/databaseadapter.cpp
  src/dataprovider.cpp
  src/doubleseries.cpp
  src/plotsettings.cpp
  src/exporterfactory.cpp
  src/recorder.cpp
  src/gui/cursor.cpp
  src/gui/discretegraph.cpp
  src/gui/interpolatinggraph.cpp
  src/gui/plotsettingsdialog.cpp
  src/gui/presentationarea.cpp
  src/gui/qcustomplot.cpp
  src/gui/selection.cpp
  src/gui/sourcedialog.cpp
  src/gui/timeline.cpp
  src/gui/timescrollbar.cpp
  src/gui/track.cpp
  src/mainwindow.cpp
  src/networkadapter.cpp
  src/sqlite.cpp
  src/stringseries.cpp
  src/value.cpp
  src/timemanager.cpp
  src/gui/startscreen.cpp
  src/exporthandler.cpp
)

set(Octopus_HEADERS
  include/abstractdataseries.h
  include/common.h
  include/CVSExporter.h
  include/exporter.h
  include/DBUtil.h
  include/recorder.h
  include/exporterfactory.h
  include/databaseadapter.h
  include/dataprovider.h
  include/doubleseries.h
  include/plotsettings.h
  include/gui/cursor.h
  include/gui/discretegraph.h
  include/gui/graph.h
  include/gui/interpolatinggraph.h
  include/gui/plotsettingsdialog.h
  include/gui/presentationarea.h
  include/gui/qcustomplot.h
  include/gui/selection.h
  include/gui/sourcedialog.h
  include/gui/timeline.h
  include/gui/timescrollbar.h
  include/gui/track.h
  include/mainwindow.h
  include/measure.h
  include/networkadapter.h
  include/serializable.h
  include/sqlite.h
  include/stringseries.h
  include/value.h
  include/visitor.h
  include/timemanager.h
  include/gui/startscreen.h
  include/exporthandler.h
)

set(Octopus_FORMS
  ui/mainwindow.ui
  ui/plotsettingsdialog.ui
  ui/sourcedialog.ui
  ui/track.ui
  ui/startscreen.ui
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

add_executable(Octopus "src/main.cpp" ${Octopus_SOURCES} ${Octopus_HEADERS}
    ${Octopus_HEADERS_MOC}
    ${Octopus_FORMS_HEADERS} 
    ${Octopus_RESOURCES_RCC})

set(Octopus_LIBRARIES EIToolkit qjson ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} ${QT_LIBRARIES})

target_link_libraries(Octopus ${Octopus_LIBRARIES})

if(FALSE)
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
