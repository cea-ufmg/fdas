add_executable(gps-read gps-read.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(gps-read ${Boost_LIBRARIES})

install(TARGETS gps-read DESTINATION bin)
