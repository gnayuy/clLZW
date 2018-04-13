
project(clLZW)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(OpenCL REQUIRED)

include_directories(${OPENCL_INCLUDE_DIRS})

set(clLZW_src main.cpp lzw.cpp lzw.h)

add_executable(clLZW ${clLZW_src})
target_link_libraries(clLZW ${OPENCL_LIBRARIES})