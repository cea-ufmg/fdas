add_executable(iio-read iio-read.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(iio-read ${Boost_LIBRARIES})

install(TARGETS iio-read DESTINATION bin)
