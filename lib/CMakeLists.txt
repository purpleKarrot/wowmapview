
add_library(wowview
  dbcfile.cpp
  mpq.cpp
  texture.cpp
  )

target_link_libraries(wowview
  ${SDL_LIBRARY}
  )
