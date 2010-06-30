
add_library(wowview
  dbcfile.cpp
  mpq.cpp
  )

target_link_libraries(wowview
  ${SDL_LIBRARY}
  )
