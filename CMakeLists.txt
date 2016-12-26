project(webserver)

cmake_minimum_required(VERSION 3.2)

SET(LIBEVENT_NAME "event" CACHE STRING "Name of the libevent library (as used in the linker argument).")
SET(LIBEVENT_LIB_DIR "/usr/local/lib" CACHE STRING "Path to libevent libraries.")
SET(LIBEVENT_INCLUDE_DIR "/usr/local/include" CACHE STRING "Path to libevent headers.")

MESSAGE("LIBEVENT_NAME: " ${LIBEVENT_NAME})
MESSAGE("LIBEVENT_LIB_DIR: " ${LIBEVENT_LIB_DIR})
MESSAGE("LIBEVENT_INCLUDE_DIR: " ${LIBEVENT_INCLUDE_DIR})

link_directories("${LIBEVENT_LIB_DIR}/")

include_directories("${LIBEVENT_INCLUDE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

add_executable(webserver webserver.cpp)
target_link_libraries(webserver ${LIBEVENT_NAME}) # event_custom # @FIXME this should be passed in via variable?
