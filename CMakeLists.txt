cmake_minimum_required (VERSION 3.0.2)
project (fdas)

find_package(Boost REQUIRED COMPONENTS program_options log)

find_library(IIO_LIBRARY iio)
find_path(IIO_INCLUDE_DIR iio.h)
include_directories("${IIO_INCLUDE_DIR}")


if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(common)
add_subdirectory(devices)
#add_subdirectory(scripts)
