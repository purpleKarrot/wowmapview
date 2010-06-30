
add_executable(schema_generator EXCLUDE_FROM_ALL parser.cpp)

file(GLOB DBC_SCHEMA *.schema)

set(SCHEMA_LIST)
foreach(INPUT ${DBC_SCHEMA})
  get_filename_component(SCHEMA_WE ${INPUT} NAME_WE)
  set(OUTPUT ${SCHEMA_WE}.hpp)

  add_custom_command(OUTPUT ${OUTPUT}
    COMMAND schema_generator ${INPUT} > ${OUTPUT}
    DEPENDS schema_generator ${INPUT})

  list(APPEND SCHEMA_LIST ${OUTPUT})
endforeach(INPUT ${DBC_SCHEMA})

add_custom_target(dbc_schema DEPENDS ${SCHEMA_LIST})
