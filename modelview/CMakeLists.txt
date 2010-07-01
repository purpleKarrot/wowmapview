project(wowmodelviewer)
cmake_minimum_required(VERSION 2.6)

set(wxWidgets_USE_LIBS base core gl net aui xml)
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})

add_definitions(-DWotLK)
add_definitions(-D_LINUX)
add_definitions(-D_LINUX32)

qt4_wrap_cpp(MOC_SOURCE
  app.h
  widgets/FileList.hpp
  widgets/CharacterControl.hpp
  widgets/EquipmentControl.hpp
  )

file(GLOB WOWMV_SOURCES *.cpp)
add_executable(wowmodelview
  ${WOWMV_SOURCES}
  ${MOC_SOURCE}
  widgets/TreeItem.cpp
  widgets/FileList.cpp
  widgets/FileListModel.cpp
  widgets/CharacterControl.cpp
  widgets/EquipmentControl.cpp
  )

target_link_libraries(wowmodelview
  ${QT_LIBRARIES}
  ${wxWidgets_LIBRARIES}
  Storm wowview
  ${GLEW_LIBRARIES}
  ${SDLMIXER_LIBRARY}
  ${QGLVIEWER_LIBRARY}
  )

add_dependencies(wowmodelview dbc_schema)
