cmake_minimum_required(VERSION 3.10.0)
project(bjudger VERSION 0.1.0 LANGUAGES C CXX)

find_package(workflow REQUIRED)

link_directories(/usr/local/lib)

set(CMAKE_CXX_STANDARD 23)
if (NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions(-DDEBUG=1)
  set(CMAKE_CXX_FLAGS "-Wall")
else()
  add_definitions(-UDEBUG=1)
  set(CMAKE_CXX_FLAGS "-O3 -Wall")
endif()

# Set rpath
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH /usr/local/lib)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost REQUIRED COMPONENTS process json program_options)

# simple cpp judger
aux_source_directory(src/lib/plugins/judgers/cpp PLUGIN_CPP_SRC)
add_library(bjudger_simple_cpp_judger SHARED ${PLUGIN_CPP_SRC})
target_link_libraries(bjudger_simple_cpp_judger PRIVATE Boost::process)

# problem and context
add_library(bjudger_problem SHARED src/lib/problem.cpp)
target_link_libraries(bjudger_problem PUBLIC crypto)
add_library(context src/lib/context.cpp)

# main: bjudger
aux_source_directory(src/main MAIN_SRC)
add_executable(bjudger ${MAIN_SRC})
include_directories(src/include)
target_link_libraries(bjudger PRIVATE workflow)
target_link_libraries(bjudger PRIVATE Boost::json Boost::program_options)
target_link_libraries(bjudger PRIVATE bjudger_problem)

# api: exist
aux_source_directory(src/lib/plugins/apis/exist EXIST_SRC)
add_library(bjudger_api_exist SHARED ${EXIST_SRC})
target_link_libraries(bjudger_api_exist PUBLIC workflow)
target_link_libraries(bjudger_api_exist PRIVATE bjudger_problem)

# api: judge
aux_source_directory(src/lib/plugins/apis/judge JUDGE_SRC)
add_library(bjudger_api_judge SHARED ${JUDGE_SRC})
target_link_libraries(bjudger_api_judge PUBLIC workflow)
target_link_libraries(bjudger_api_judge PRIVATE bjudger_problem)

# sandbox: bsdbx
add_subdirectory(third_party/bsdbx ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Add install target, install the binary to /usr/local/bin and libraries to /usr/local/lib
install(TARGETS bjudger bjudger_problem context bjudger_simple_cpp_judger bjudger_api_exist bjudger_api_judge bsdbx
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# Install etc/config.json to /etc/bjudger/config.json
install(FILES etc/config.json DESTINATION etc/bjudger)

# Install example to /usr/share/bjudger/example
install(DIRECTORY example DESTINATION share/bjudger)
