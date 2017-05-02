add_executable(gps-read gps-read.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(gps-read boost_program_options boost_log)

install(TARGETS gps-read DESTINATION bin)
