cmake_minimum_required(VERSION 3.8)

project(KP)

set(CMAKE_CXX_STANDART 17) #стандарт языка си++
set(CMAKE_CXX_STANDART_REQUIRED ON) #без соответствия стандарту не будет собран

find_package(Threads)

add_executable(client client.cpp game_functions.hpp)
add_executable(server server.cpp game_functions.hpp)

target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT} rt)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} rt)