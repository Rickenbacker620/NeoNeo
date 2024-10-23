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
        engine_base.ixx
        hook.ixx
        hook_param.ixx
        common.ixx
        pcengine.ixx
)
target_link_libraries(neohooklibs PUBLIC cppzmq minhook::minhook)
