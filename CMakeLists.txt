
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(wowmapview)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DGL_GLEXT_PROTOTYPES)

set(QT_USE_QTOPENGL ON)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

qt4_wrap_cpp(MOC_SOURCE
  src/widgets/MainWindow.hpp
  src/widgets/FileList.hpp
  )

file(COPY src/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)

add_executable(wowmapview
  ${MOC_SOURCE}
  src/lib/texture.cpp
  src/widgets/FileList.cpp
  src/widgets/MapItemModel.cpp
  src/widgets/MainWindow.cpp
  src/widgets/RenderWidget.cpp
  src/areadb.cpp
  src/dbcfile.cpp
  src/frustum.cpp
  src/liquid.cpp
  src/maptile.cpp
  src/model.cpp
  src/mpq_stormlib.cpp
  src/particle.cpp
  src/shaders.cpp
  src/sky.cpp
  src/wmo.cpp
  src/world.cpp
  src/wowmapview.cpp
  )

target_link_libraries(wowmapview
  ${QT_LIBRARIES}
  Storm
  )
