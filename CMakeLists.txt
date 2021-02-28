cmake_minimum_required(VERSION 3.0)
project(chord)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

#include_directories(.)
#include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_library(YAMLCPP libyaml-cpp.a)
message("***", ${YAMLCPP})

set(LIB_SRC
	chord/logger.cpp
	chord/util.cpp
	chord/config.cpp
	)
	
add_library(chord SHARED ${LIB_SRC})
#解决文件路径名在log中的显示过长的问题
force_redefine_file_macro_for_sources(chord) #__FILE__ 
#add_library(chord_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERITES (chord_static PROPERTIES OUTPUT_NAME "chord")

add_executable(test01 tests/test.cc)
add_dependencies(test01 chord)
force_redefine_file_macro_for_sources(test01) #__FILE__
target_link_libraries(test01 chord ${YAMLCPP})


add_executable(test_config tests/testconfig.cc)
force_redefine_file_macro_for_sources(test_config) #__FILE__
add_dependencies(test_config chord)
target_link_libraries(test_config chord ${YAMLCPP})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
