project(wowmodelviewer)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(wxWidgets_USE_LIBS base core gl net aui xml)
find_package(wxWidgets REQUIRED )
find_package(GLEW REQUIRED)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(USE_STORM 1)

include(${wxWidgets_USE_FILE})
include(cmake/sources.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${BZIP2_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${JPEG_INCLUDE_DIR}
    ${PNG_PNG_INCLUDE_DIR}
)

add_definitions(
    -DWotLK
)

add_library(cximage STATIC ${CXIMAGE_SOURCES})

add_definitions(-D_LINUX)
add_definitions(-D_LINUX32)

add_executable(wowmodelviewer ${WOWMV_SOURCES})

target_link_libraries(wowmodelviewer
    ${EXTRA_LIBS}
    cximage Storm
    ${wxWidgets_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARY}
)
