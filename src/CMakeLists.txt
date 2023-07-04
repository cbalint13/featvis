##
## License: GPLv3
## https://www.gnu.org/licenses/gpl-3.0.en.html
##
## Copyright 2012, 2023
##       Cristian Balint < cristian dot balint at gmail dot com >
##

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(FEATVIS_SRCS
    main.cpp
    mainwindow.cpp
    qtosgwidget.cpp
    osghdf.cpp
)

include_directories(${HDF4_INCLUDE_DIRS})
qt_add_resources(FEATVIS_SRCS mainwindow.qrc)

add_executable(featvis ${FEATVIS_SRCS})
target_link_libraries(featvis
  PUBLIC
    Qt5::Widgets
    ${OPENSCENEGRAPH_LIBRARIES}
    ${HDF4_LIBRARIES}
)
