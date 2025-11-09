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
    include/merc/merc.h
    include/merc/merc-database.h
    source/merc.cpp
    source/merc-database.cpp
)
target_include_directories(merc PUBLIC include)
target_link_libraries(merc PUBLIC interface-database script av-realtime)
if(WIN32)
    target_sources(merc PRIVATE
    )
elseif(APPLE)
    set(OBJC_SOURCES
    )
    target_sources(merc PRIVATE ${OBJC_SOURCES})
    set_source_files_properties(${OBJC_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
endif()
