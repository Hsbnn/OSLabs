cmake_minimum_required(VERSION 3.8)
#требования к версии

project(OS4)
#объявляет новый проект с именем OS4

set(CMAKE_CXX_STANDART 20) #стандарт языка си++
set(CMAKE_CXX_STANDART_REQUIRED ON) #без соответствия стандарту не будет собран, гарантирует, что будет использован указанный стандарт.

add_executable(dynamic_OS4 maindynamic.cpp)
add_executable(OS4 main.cpp)   

add_library(SinIntegral SHARED SinIntegral.cpp)
#объявляет динамическую библиотеку с именем SinIntegral
add_library(Pi SHARED Pi.cpp)
#CMake компилирует эти файлы и создает динамическую библиотеку с расширением ".so"

target_link_libraries(dynamic_OS4 PRIVATE -ldl)
#связывание только с dynamic_OS4

target_link_libraries(dynamic_OS4 PRIVATE SinIntegral Pi)
#библиотеки, которые должны быть связаны с основным исполняемым файлом для его сборки

target_link_libraries(OS4 PRIVATE SinIntegral Pi)
#./OS4 - запуск main программы, объявления основного исполняемого файла
