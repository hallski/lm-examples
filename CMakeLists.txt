cmake_minimum_required(VERSION 2.6)

project(LmExamples C)

include(Warnings.cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LMDEBUG REQUIRED loudmouth-1.0)

add_executable(simple-gtalk simple-gtalk.c)
target_link_libraries(simple-gtalk ${LMDEBUG_LIBRARIES})

include_directories(${LMDEBUG_INCLUDE_DIRS})
link_directories(${LMDEBUG_LIBRARY_DIRS})

