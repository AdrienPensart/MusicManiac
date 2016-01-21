cmake_minimum_required(VERSION 2.8.11)
project(musicmaniac C CXX)

#add_subdirectory(src/taglib)
find_path(UUID_INCLUDE_DIR uuid/uuid.h)
find_library(UUID_LIBRARY NAMES uuid PATH)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.60 REQUIRED COMPONENTS filesystem system)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
find_package(FUSE REQUIRED)

IF(NOT UUID_LIBRARY)
	SET(UUID_LIBRARY "")
ENDIF(NOT UUID_LIBRARY)

IF(UUID_INCLUDE_DIR)
	SET(UUID_FOUND TRUE)
ENDIF(UUID_INCLUDE_DIR)

IF(NOT UUID_FOUND)
	IF(UUID_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find UUID")
	ENDIF(UUID_FIND_REQUIRED)
ENDIF(NOT UUID_FOUND)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)
file(GLOB INOTIFY_SRC_FILES src/inotify/*.h)
file(GLOB LIB_SRC_FILES src/lib/*.cpp src/lib/*.hpp)
file(GLOB FS_SRC_FILES src/fs/*.cpp src/fs/*.hpp)
file(GLOB MUSICMANIAC_SRC_FILES src/musicmaniac/*.cpp src/musicmaniac/*.hpp)
file(GLOB DB_SRC_FILES src/db/*.cpp src/db/*.hpp)
file(GLOB COMMON_SRC_FILES src/common/*.hpp)

file(GLOB UI_FILES src/musicmaniac/*.ui)
file(GLOB RES_FILES src/musicmaniac/*.qrc)
QT5_WRAP_UI(UI_HEADERS ${UI_FILES})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RES_FILES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/lib)
#include_directories(${CMAKE_SOURCE_DIR}/src/taglib)
#include_directories(${CMAKE_SOURCE_DIR}/src/taglib/taglib)

# Includes Catch in the project:
set(CATCH_INCLUDE_DIR "src/catch/include" CACHE INTERNAL "Path to include folder for Catch")
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.

# Main target
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DFUSE_USE_VERSION=30 -D_FILE_OFFSET_BITS=64 -Wnon-virtual-dtor")
add_executable(musicmaniac ${MUSICMANIAC_SRC_FILES} ${FS_SRC_FILES} ${DB_SRC_FILES} ${INOTIFY_SRC_FILES} ${COMMON_SRC_FILES} ${LIB_SRC_FILES} ${UI_HEADERS} ${RESOURCES_RCC})
target_include_directories(musicmaniac PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(musicmaniac Qt5::Widgets Qt5::Sql uuid tag ${Boost_LIBRARIES} ${FUSE_LIBRARIES})

add_executable(cta test/test.cpp)
add_test(cta cta)

