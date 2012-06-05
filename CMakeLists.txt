# See http://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
cmake_minimum_required(VERSION 2.6)
project(Octopus)

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

find_package(Qt4 REQUIRED)

set(Octopus_SOURCES
  src/mainwindow.cpp
  src/abstractdataseries.cpp
  src/stringseries.cpp
  src/doubleseries.cpp
  src/dataprovider.cpp
  src/value.cpp
  src/gui/discretegraph.cpp
  src/gui/interpolatinggraph.cpp
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

set(Octopus_HEADERS
  include/mainwindow.h
  include/common.h
  include/abstractdataseries.h
  include/serializable.h
  include/stringseries.h
  include/doubleseries.h
  include/dataprovider.h
  include/value.h
  include/visitor.h
  include/gui/graph.h
  include/gui/discretegraph.h
  include/gui/interpolatinggraph.h
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
set(Octopus_FORMS
  ui/mainwindow.ui
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

add_executable(Octopus "src/main.cpp" ${Octopus_SOURCES} ${Octopus_HEADERS}
    ${Octopus_HEADERS_MOC}
    ${Octopus_FORMS_HEADERS} 
    ${Octopus_RESOURCES_RCC})
target_link_libraries(Octopus ${QT_LIBRARIES})

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
target_link_libraries(SampleTest gtest gtest_main ${QT_LIBRARIES})

# add_test(<TEST_NAME> <BINARY>)
add_test(AllTestsInSampleTest SampleTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS SampleTest)
