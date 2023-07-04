##
## License: GPLv3
## https://www.gnu.org/licenses/gpl-3.0.en.html
##
## Copyright 2012, 2023
##       Cristian Balint < cristian dot balint at gmail dot com >
##

cmake_minimum_required(VERSION 3.20)

project(featvis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(include/)

# find QT5 widgets
find_package(Qt5Widgets REQUIRED)
# find OpenSceneGraph
find_package(OpenSceneGraph REQUIRED osgDB osgUtil osgViewer osgGA)

# find HDF4 headers
find_path(HDF4_INCLUDE_DIR hdf/hdf.h REQUIRED
          HINTS ${PC_HDF4_INCLUDEDIR} ${PC_HDF4_INCLUDE_DIRS}
          PATH_SUFFIXES HDF4)
set(HDF4_INCLUDE_DIRS ${HDF4_INCLUDE_DIR}/hdf)
message(STATUS "HDF4 includes: " ${HDF4_INCLUDE_DIR})

# find HDF4 libraries
find_library(HDF4_DF_LIBRARY NAMES libdf.so HDF4 REQUIRED)
find_library(HDF4_MHDF_LIBRARY NAMES libmfhdf.so HDF4 REQUIRED)
list(APPEND HDF4_LIBRARIES ${HDF4_MHDF_LIBRARY} ${HDF4_DF_LIBRARY})
message(STATUS "HDF4 libraries: " ${HDF4_LIBRARIES})

# FeatVIS GUI
add_subdirectory(src)
# Vector Tools
add_subdirectory(src/tools)
