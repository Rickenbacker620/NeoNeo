

# add_library(dialogue STATIC dialogue.cpp)
# target_include_directories(dialogue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(dialogue PRIVATE fmt::fmt)

# add_library(server STATIC server.cpp)
# target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(server PRIVATE cppzmq fmt::fmt)

# add_library(hook_param STATIC hook_param.cpp)
# target_include_directories(hook_param PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add_library(neohook MODULE main.cpp)

# target_link_libraries(neohook PRIVATE minhook::minhook fmt::fmt dialogue server hook_param)

add_library(neohook MODULE)

target_compile_features(neohook PUBLIC cxx_std_23)

target_compile_options(neohook PUBLIC /MP /W4)

target_compile_definitions(neohook PRIVATE WIN32_LEAN_AND_MEAN)


target_sources(neohook
       PUBLIC
       FILE_SET CXX_MODULES FILES
       main.ixx
)

target_link_libraries(neohook PUBLIC neohooklibs)

add_library(neohooklibs STATIC)

target_sources(neohooklibs
        PUBLIC
        FILE_SET CXX_MODULES FILES
        server.ixx
        engine_base.ixx
        hook.ixx
        dialogue.ixx
        hook_param.ixx
        neo_output.ixx
        logger.ixx
        common.ixx
        pcengine.ixx
)

target_link_libraries(neohooklibs PRIVATE fmt::fmt cppzmq
        PUBLIC minhook::minhook)

