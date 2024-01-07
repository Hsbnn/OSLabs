cmake_minimum_required(VERSION 3.5)
project(57Lab)

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 20)
find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)
find_path(ZeroMQ_INCLUDE_DIR NAMES zmq.hpp PATHS ${PC_ZeroMQ_INCLUDE_DIRS})

find_library(ZeroMQ_LIBRARY NAMES zmq PATHS ${PC_ZeroMQ_LIBRARY_DIRS})

add_executable(server server.cpp)
target_include_directories(server PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(server PRIVATE ${ZeroMQ_LIBRARY})

add_executable(client client.cpp)
target_include_directories(client PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(client PRIVATE ${ZeroMQ_LIBRARY})
