add_executable(iio-read iio-read.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(iio-read boost_program_options boost_log boost_exception boost_locale boost_system)

install(TARGETS iio-read DESTINATION bin)
