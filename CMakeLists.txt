cmake_minimum_required(VERSION 2.8.11)
project(musicmaniac CXX)

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
file(GLOB LIB_SRC_FILES src/lib/*.cpp src/lib/*.hpp)
file(GLOB CMD_SRC_FILES src/cmd/*.cpp src/cmd/*.hpp)
file(GLOB GUI_SRC_FILES src/gui/*.cpp src/gui/*.hpp)
file(GLOB UI_FILES src/gui/*.ui)
file(GLOB RES_FILES src/gui/*.qrc)
QT5_WRAP_UI(UI_HEADERS ${UI_FILES})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RES_FILES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/lib)

# Main target
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(musicmaniac ${GUI_SRC_FILES} ${LIB_SRC_FILES} ${UI_HEADERS} ${RESOURCES_RCC})
target_link_libraries(musicmaniac Qt5::Widgets uuid tag ${Boost_LIBRARIES})
#CMAKE 3.1 only : target_compile_features(musicmaniac PRIVATE cxx_range_for)

add_executable(musiccmd ${CMD_SRC_FILES} ${LIB_SRC_FILES})
target_link_libraries(musiccmd uuid tag ${Boost_LIBRARIES})

# Test target
#add_executable(musicmaniac_test test/test.cpp ${SRC_FILES} ${UI_HEADERS} ${RESOURCES_RCC})
#target_link_libraries(musicmaniac_test Qt5::Widgets uuid tag ${Boost_LIBRARIES})

#enable_testing()
#add_test(catch musicmaniac_test)
