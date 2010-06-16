
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(wowmapview)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DGL_GLEXT_PROTOTYPES)

set(QT_USE_QTOPENGL ON)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


include_directories(include)

add_subdirectory(lib)
add_subdirectory(mapview)
add_subdirectory(modelview)


file(COPY
    data/enUS
    data/shaders
    data/ridable.csv
  DESTINATION
    ${CMAKE_BINARY_DIR}/bin
  )

