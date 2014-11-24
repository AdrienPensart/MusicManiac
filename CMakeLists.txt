cmake_minimum_required(VERSION 2.8.11)
# innocence is entirely in C++ (or almost) !
project(musicman CXX)

FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h)
FIND_LIBRARY(UUID_LIBRARY NAMES uuid PATH)

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
QT5_WRAP_UI(UI_HEADERS ${UI_FILES})

add_executable(musicman ${SRC_FILES} ${UI_HEADERS})
target_link_libraries(musicman Qt5::Widgets uuid tag)
