cmake_minimum_required(VERSION 3.8)
#требования к версии

project(fLROS)
#имя проекта

set(CMAKE_CXX_STANDART 17) #стандарт языка си++
set(CMAKE_CXX_STANDART_REQUIRED ON) #без соответствия стандарту не будет собран

add_executable(main1 main.cpp)
add_executable(child1 Child1.cpp)
#add_executable(child2 Child2.cpp)