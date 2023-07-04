##
## License: GPLv3
## https://www.gnu.org/licenses/gpl-3.0.en.html
##
## Copyright 2012, 2023
##       Cristian Balint < cristian dot balint at gmail dot com >
##

# find GDAL
find_package( GDAL COMPONENTS REQUIRED)
if(GDAL_FOUND)
  include_directories(${GDAL_INCLUDE_DIRS})
  MESSAGE(STATUS "GDAL_LIBRARIES is: " ${GDAL_LIBRARIES})
endif()

# find ZLIB library
find_package( ZLIB COMPONENTS REQUIRED)
if(ZLIB_FOUND)
  include_directories(${ZLIB_INCLUDE_DIRS})
  MESSAGE(STATUS "ZLIB_INCLUDE_DIRS are: " ${ZLIB_INCLUDE_DIRS})
  MESSAGE(STATUS "ZLIB_LIBRARIES is: " ${ZLIB_LIBRARIES})
endif()

# find JPEG library
find_package( JPEG COMPONENTS REQUIRED)
if(JPEG_FOUND)
  include_directories(${JPEG_INCLUDE_DIRS})
  MESSAGE(STATUS "JPEG_INCLUDE_DIRS are: " ${JPEG_INCLUDE_DIRS})
  MESSAGE(STATUS "JPEG_LIBRARIES is: " ${JPEG_LIBRARIES})
endif()

# find HDF4 library
find_path(HDF4_INCLUDE_DIR hdf/hdf.h
          HINTS ${PC_HDF4_INCLUDEDIR} ${PC_HDF4_INCLUDE_DIRS}
          PATH_SUFFIXES HDF4 )
include_directories(${HDF4_INCLUDE_DIR}/hdf)
set(HDF4_INCLUDE_DIRS ${HDF4_INCLUDE_DIR} )
MESSAGE(STATUS "HDF4_INCLUDE_DIR is: " ${HDF4_INCLUDE_DIR})

find_library(HDF4_MHDF_LIBRARY NAMES libmfhdf.so HDF4)
set(HDF4_LIBRARIES ${HDF4_MHDF_LIBRARY} )
find_library(HDF4_DF_LIBRARY NAMES libdf.so HDF4)
set(HDF4_LIBRARIES ${HDF4_LIBRARIES} ${HDF4_DF_LIBRARY} )
MESSAGE(STATUS "HDF4_LIBRARIES is: " ${HDF4_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set HDF4_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(HDF4 DEFAULT_MSG
                                  HDF4_LIBRARIES HDF4_INCLUDE_DIR)
mark_as_advanced(HDF4_INCLUDE_DIR HDF4_LIBRARIES )

# find FLANN library
find_path(FLANN_INCLUDE_DIR flann/flann.h
          HINTS ${PC_FLANN_INCLUDEDIR} ${PC_FLANN_INCLUDE_DIRS}
          PATH_SUFFIXES FLANN )
include_directories(${FLANN_INCLUDE_DIR}/flann)
set(FLANN_INCLUDE_DIRS ${FLANN_INCLUDE_DIR} )
MESSAGE(STATUS "FLANN_INCLUDE_DIR is: " ${FLANN_INCLUDE_DIR})

find_library(FLANN_LIBRARY NAMES flann FLANN)
set(FLANN_LIBRARIES ${FLANN_LIBRARY} )
MESSAGE(STATUS "FLANN_LIBRARIES is: " ${FLANN_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FLANN_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FLANN  DEFAULT_MSG
                                  FLANN_LIBRARIES FLANN_INCLUDE_DIR)
mark_as_advanced(FLANN_INCLUDE_DIR FLANN_LIBRARIES )

#use pkg-config to find various modues
INCLUDE(FindPkgConfig REQUIRED)

# find GSL
IF(PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
    PKG_CHECK_MODULES(GSL gsl)
    include_directories(${GSL_INCLUDE_DIRS})
ENDIF()

# find FFTW3F
IF(PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
    PKG_CHECK_MODULES(FFTW3F fftw3f)
    include_directories(${FFTW3F_INCLUDE_DIRS})
ENDIF()


# featmatrix
add_executable(featmatrix
               featmatrix.c
               hdfdata.c
               matrix.c
               raster.c)
target_link_libraries(featmatrix
           ${GSL_LIBRARIES}
           ${GDAL_LIBRARIES}
           ${FLANN_LIBRARIES}
           ${HDF4_LIBRARIES}
           ${ZLIB_LIBRARIES}
           ${JPEG_LIBRARIES} m tirpc)

# featmds
add_executable(featmds
               featmds.c
               hdfdata.c
               mds.c
               matrix.c
               raster.c)
target_link_libraries(featmds
           ${GSL_LIBRARIES}
           ${GDAL_LIBRARIES}
           ${HDF4_LIBRARIES}
           ${ZLIB_LIBRARIES}
           ${JPEG_LIBRARIES} tirpc)

# featsne
add_executable(featsne
               featsne.cpp
               hdfdata.c
               matrix.c
               raster.c
               bhtsne/sptree.cpp
               bhtsne/tsne.cpp)
target_link_libraries(featsne
           ${GSL_LIBRARIES}
           ${GDAL_LIBRARIES}
           ${HDF4_LIBRARIES}
           ${ZLIB_LIBRARIES}
           ${JPEG_LIBRARIES} tirpc)
target_include_directories(featsne PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bhtsne>)

# vsegment
add_executable(vsegment
               vsegment.c
               hdfdata.c
               matrix.c
               raster.c)
target_link_libraries(vsegment
           ${FLANN_LIBRARIES}
           ${GSL_LIBRARIES}
           ${GDAL_LIBRARIES}
           ${HDF4_LIBRARIES}
           ${ZLIB_LIBRARIES}
           ${JPEG_LIBRARIES} tirpc)

# findgist
add_executable(findgist
               gist/main.c
               gist/gist.c
               gist/hdfdata.c
               gist/raster.c
               gist/standalone_image.c)
target_link_libraries(findgist
           ${FFTW3F_LIBRARIES}
           ${GSL_LIBRARIES}
           ${GDAL_LIBRARIES}
           ${HDF4_LIBRARIES}
           ${ZLIB_LIBRARIES}
           ${JPEG_LIBRARIES} tirpc)
set_target_properties(findgist PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -DUSE_GIST -DSTANDALONE_GIST" )
