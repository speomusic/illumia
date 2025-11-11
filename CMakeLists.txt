cmake_minimum_required(VERSION 3.25)
project(illumia)

if(NOT COMPILE_COMMANDS_RUN)
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
set(VST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/vst3sdk/public.sdk/source/vst)

add_library(illumia STATIC
    include/illumia/space.h
    source/faust.cpp
    source/space.cpp
    source/vstvideoeffect.cpp
    ${VST_SOURCE_DIR}/moduleinfo/moduleinfoparser.cpp
    ${VST_SOURCE_DIR}/moduleinfo/moduleinfocreator.cpp
)
target_include_directories(illumia PUBLIC include)
target_link_libraries(illumia PUBLIC sdk sdk_hosting)
if(WIN32)
    target_sources(illumia PRIVATE
        ${VST_SOURCE_DIR}/hosting/module_win32.cpp
        include/illumia/windows-utils.h
        source/graphics.cpp
        source/paths.cpp
        source/windows-utils.cpp
    )
elseif(APPLE)
    set(OBJC_SOURCES
        ${VST_SOURCE_DIR}/hosting/module_mac.mm
        source/graphics.mm
        source/paths.mm
    )
    target_sources(illumia PRIVATE ${OBJC_SOURCES})
    set_source_files_properties(${OBJC_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(illumia INTERFACE
        "-framework Metal"
        "-framework MetalKit"
        "-framework MetalPerformanceShaders"
    )
endif()

add_subdirectory(modules)

enable_testing()
add_subdirectory(test)
