cmake_minimum_required(VERSION 2.8...3.13)

#if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
# set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")
#else()
   set(VCPKG_TARGET_TRIPLET "x86-windows-static-md")
#endif()

project(NeoNeo
        VERSION 0.0.1
        DESCRIPTION "A Neo Galgame Translator"
        LANGUAGES CXX)

enable_testing()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

find_package(minhook CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(Mecab CONFIG REQUIRED)

include(GoogleTest)
include(CTest)

add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(injector)