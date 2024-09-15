add_executable(injector main.cpp)

target_compile_features(injector PUBLIC cxx_std_20)

target_compile_options(injector PUBLIC /MP /W4 /source-charset:utf-8 /execution-charset:utf-8)


target_compile_definitions(injector PRIVATE WIN32_LEAN_AND_MEAN UNICODE _UNICODE)