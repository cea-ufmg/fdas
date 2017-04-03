add_executable(gps-read gps-read.cpp $<TARGET_OBJECTS:common>)
install(TARGETS gps-read DESTINATION bin)
