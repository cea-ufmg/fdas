add_executable(iio-read iio-read.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(iio-read boost_program_options boost_log)

install(TARGETS iio-read DESTINATION bin)
