cmake_minimum_required(VERSION 3.10.0)
project(bjudger VERSION 0.1.0 LANGUAGES C CXX)

find_package(workflow REQUIRED)

set(CMAKE_CXX_STANDARD 23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost REQUIRED COMPONENTS filesystem process)

aux_source_directory(src/lib/plugins/cpp PLUGIN_CPP_SRC)
add_library(simple_cpp_judger SHARED ${PLUGIN_CPP_SRC})
target_link_libraries(simple_cpp_judger PRIVATE Boost::filesystem Boost::process)

add_executable(bjudger src/main.cpp)
include_directories(src/include)
target_link_libraries(bjudger PRIVATE workflow)
target_link_libraries(bjudger PRIVATE Boost::filesystem Boost::process)

add_subdirectory(third_party/bsdbx ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)