
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(wowmapview)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_definitions(-DGL_GLEXT_PROTOTYPES)

set(QT_USE_QTOPENGL ON)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


find_package(SDL       REQUIRED)
find_package(SDL_mixer REQUIRED)
include_directories(${SDL_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR})


include_directories(include)

add_subdirectory(dbc)
include_directories(${CMAKE_BINARY_DIR}/dbc)


add_subdirectory(lib)
add_subdirectory(mapview)
add_subdirectory(modelview)


file(COPY
    data/enUS
    data/shaders
    data/ridable.csv
  DESTINATION
    ${CMAKE_BINARY_DIR}
  )
