

add_library(dialogue STATIC dialogue.cpp)
target_include_directories(dialogue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dialogue PRIVATE fmt::fmt)

add_library(server STATIC server.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server PRIVATE cppzmq fmt::fmt)

add_library(hook_param STATIC hook_param.cpp)
target_include_directories(hook_param PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hook_param PRIVATE ctre::ctre)

add_library(neohook MODULE main.cpp hook.cpp engines/engine_base.cpp engines/PC/engine.cpp engines/kirikiri/engine.cpp)
# add_library(neohook MODULE main.cpp)

target_compile_features(neohook PUBLIC cxx_std_23)

target_compile_options(neohook PUBLIC /MP /W4)

target_compile_definitions(neohook PRIVATE WIN32_LEAN_AND_MEAN)

target_link_libraries(neohook PRIVATE minhook::minhook ctre::ctre fmt::fmt dialogue server ws2_32)
