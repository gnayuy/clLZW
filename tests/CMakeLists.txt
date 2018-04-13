
project(tiffimageio)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(TIFF REQUIRED)
include_directories(${TIFF_INCLUDE_DIR})

set(tiffimageio_src tiffimageio.cpp)

add_executable(tiffimageio ${tiffimageio_src})
target_link_libraries(tiffimageio ${TIFF_LIBRARIES})
