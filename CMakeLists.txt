
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(wowmapview)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(SDL)
include_directories(${SDL_INCLUDE_DIR})

file(COPY src/shaders src/arial.info src/arial.tga
  DESTINATION ${CMAKE_BINARY_DIR}/bin
  )

add_executable(wowmapview
  src/lib/texture.cpp
  src/areadb.cpp
  src/dbcfile.cpp
  src/font.cpp
  src/frustum.cpp
  src/liquid.cpp
  src/maptile.cpp
  src/menu.cpp
  src/model.cpp
  src/mpq_stormlib.cpp
  src/particle.cpp
  src/shaders.cpp
  src/sky.cpp
  src/test.cpp
  src/video.cpp
  src/wmo.cpp
  src/world.cpp
  src/wowmapview.cpp
  )

target_link_libraries(wowmapview
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARY}
  Storm
  )