find_library(libiio_LIBRARY iio)
find_path(libiio_INCLUDE_DIR iio.h)

if(libiio_INCLUDE_DIR AND libiio_LIBRARY)
  include_directories("${libiio_INCLUDE_DIR}")

  add_executable(iio-read iio-read.cpp $<TARGET_OBJECTS:common>)
  target_link_libraries(iio-read ${Boost_LIBRARIES} ${libiio_LIBRARY})

  install(TARGETS iio-read DESTINATION bin)
endif(libiio_INCLUDE_DIR AND libiio_LIBRARY)
