cmake_minimum_required(VERSION 2.8...3.13)

project(NeoNeo
        VERSION 0.0.1
        DESCRIPTION "A Neo Galgame Translator"
        LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG f5485b8454544c2f034c78f8f127c1d03dea3636
    GIT_SHALLOW 1
)

FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG v3.8.1
    GIT_SHALLOW 1
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
    GIT_SHALLOW 1
)

FetchContent_MakeAvailable(minhook ctre fmt)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_subdirectory(src)

add_subdirectory(injector)