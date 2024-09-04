add_library(neohook MODULE main.cpp hook.cpp dialogue.cpp engines/engine_base.cpp engines/PC/engine.cpp engines/kirikiri/engine.cpp)

target_compile_features(neohook PUBLIC cxx_std_23)

target_compile_options(neohook PUBLIC /MP /W4)

target_compile_definitions(neohook PRIVATE WIN32_LEAN_AND_MEAN)

# target_precompile_headers(
#   neohook
#   PRIVATE
#   <windows.h>
#   <WS2tcpip.h>
#   <winsock2.h>
#   <algorithm>
#   <array>
#   <atomic>
#   <cassert>
#   <concrt.h>
#   <chrono>
#   <cstdint>
#   <deque>
#   <filesystem>
#   <functional>
#   <iostream>
#   <memory>
#   <mutex>
#   <optional>
#   <regex>
#   <string>
#   <thread>
#   <tuple>
#   <unordered_map>
#   <unordered_set>
#   <vector>
#   <type_traits>
#   <MinHook.h>
#   ${CMAKE_SOURCE_DIR}/include/common.h)

target_precompile_headers(neohook PRIVATE ${CMAKE_SOURCE_DIR}/include/common.h)

target_link_libraries(neohook PRIVATE minhook ws2_32 ctre fmt)
