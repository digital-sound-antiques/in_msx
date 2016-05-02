cmake_minimum_required(VERSION 3.4)

add_subdirectory(modules/libkss)

include_directories(
	src 
	modules/libkss/src 
	modules/libkss/modules/emu2149 
	modules/libkss/modules/emu2413
	modules/libkss/modules/emu2212
	modules/libkss/modules/emu8950
	modules/libkss/modules/emu76489
	modules/libkss/modules/kmz80
)

# find_library(WIN32_COMCTRL comctl32.lib)

file(GLOB SOURCES src/*.c src/**/*.c)

add_library(in_msx SHARED ${SOURCES} src/rc/resource.rc)
target_link_libraries(in_msx kss comctl32)
