cmake_minimum_required(VERSION 3.4)

set(CMAKE_C_FLAGS "/Ox /W3 /wd4996")
set(configurations
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
)
foreach(flag_var ${configurations})
  if(${flag_var} MATCHES "/MD")
    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
  endif(${flag_var} MATCHES "/MD")
endforeach()

add_subdirectory(modules/libkss)

include_directories(
	src 
	modules/libkss/src 
	modules/libkss/modules/ 
)

# find_library(WIN32_COMCTRL comctl32.lib)

file(GLOB SOURCES src/*.c src/**/*.c)

add_library(in_msx SHARED ${SOURCES} src/rc/resource.rc)
target_link_libraries(in_msx kss comctl32)
