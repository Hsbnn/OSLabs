cmake_minimum_required(VERSION 3.8)
#требования к версии

project(OS4)
#имя проекта
#объявляет новый проект с именем OS4

set(CMAKE_CXX_STANDART 20) #стандарт языка си++
set(CMAKE_CXX_STANDART_REQUIRED ON) #без соответствия стандарту не будет собран, гарантирует, что будет использован указанный стандарт.

add_executable(dynamic_OS4 maindynamic.cpp)  
add_library(SinIntegral SHARED SinIntegral.cpp)
#объявляет динамическую библиотеку с именем SinIntegral
#Файл SinIntegral.cpp содержит реализацию функций, которые будут упаковываться в динамическую библиотеку
add_library(Pi SHARED Pi.cpp)
#CMake компилирует эти файлы и создает динамическую библиотеку с расширением ".so"

add_executable(OS4 main.cpp SinIntegral.cpp Pi.cpp) 
#функция указывает CMake, какие исходные файлы должны быть скомпилированы для создания исполняемого файла OS4
#OS4 - запуск main программы, объявления основного исполняемого файла
target_link_libraries(dynamic_OS4 PRIVATE -ldl)
#связывание только с dynamic_OS4
#-ldl обеспечивает функционал для работы с динамическими библиотеками в Linux
target_link_libraries(dynamic_OS4 PRIVATE SinIntegral Pi)
#библиотеки, которые должны быть связаны с основным исполняемым файлом для его сборки