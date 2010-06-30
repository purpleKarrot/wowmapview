
qt4_wrap_cpp(MOC_SOURCE
  widgets/MainWindow.hpp
  widgets/FileList.hpp
  )

#file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)

add_executable(wowmapview
  ${MOC_SOURCE}
  lib/texture.cpp
  widgets/FileList.cpp
  widgets/MapItemModel.cpp
  widgets/MainWindow.cpp
  widgets/RenderWidget.cpp
  areadb.cpp
  frustum.cpp
  liquid.cpp
  maptile.cpp
  model.cpp
  particle.cpp
  shaders.cpp
  sky.cpp
  wmo.cpp
  world.cpp
  wowmapview.cpp
  )

target_link_libraries(wowmapview
  ${QT_LIBRARIES}
  Storm wowview
  )

add_dependencies(wowmapview dbc_schema)
