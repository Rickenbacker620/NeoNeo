cmake_minimum_required(VERSION 2.8...3.13)

project(NeoNeo
        VERSION 0.0.1
        DESCRIPTION "A Neo Galgame Translator"
        LANGUAGES CXX)

enable_testing()

find_package(fmt CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

include(GoogleTest)
include(CTest)

add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(injector)