cmake_minimum_required(VERSION 2.8.11)
# innocence is entirely in C++ (or almost) !
project(musicman CXX)

FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h)
FIND_LIBRARY(UUID_LIBRARY NAMES uuid PATH)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.55.0 COMPONENTS filesystem system)

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
find_package(Qt5Widgets)
file(GLOB SRC_FILES *.cpp *.hpp)
file(GLOB UI_FILES *.ui)
file(GLOB RES_FILES *.qrc)
QT5_WRAP_UI(UI_HEADERS ${UI_FILES})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RES_FILES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Debug mode enabled")
	add_definitions(-DINNOCENCE_DEBUG)
else()
	message(STATUS "Release mode enabled")
	add_definitions(-DINNOCENCE_DEBUG)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(system)
add_subdirectory(network)
add_subdirectory(log)

add_executable(musicman ${SRC_FILES} ${UI_HEADERS} ${RESOURCES_RCC})
target_link_libraries(musicman Qt5::Widgets uuid tag system network log ${Boost_LIBRARIES})

