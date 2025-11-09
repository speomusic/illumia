cmake_minimum_required(VERSION 3.25)
project(illumia)

if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(COMMAND cmake --preset compile-commands COMMAND_ECHO STDERR)
endif()

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    add_compile_definitions(UNICODE NOMINMAX)
elseif(APPLE)
    add_compile_definitions($<$<CONFIG:Debug>:DEVELOPMENT>)
    add_compile_options("-fvisibility=hidden" "-Wall")
endif()

add_subdirectory(vst3sdk)
target_link_libraries(sdk INTERFACE "-framework Cocoa")

add_library(illumia STATIC
    include/illumia/space.h
    source/space.cpp
)
target_include_directories(illumia PUBLIC include)
target_link_libraries(illumia PUBLIC sdk sdk_hosting)

add_subdirectory(modules)

enable_testing()
add_subdirectory(test)
