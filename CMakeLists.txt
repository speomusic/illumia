cmake_minimum_required(VERSION 3.25)
project(merc)

if(NOT MERC_COMPILE_COMMANDS_RUN)
    execute_process(COMMAND cmake --preset compile-commands COMMAND_ECHO STDERR)
    file(COPY_FILE build-compile-commands/compile_commands.json compile_commands.json)
    execute_process(COMMAND sed -i s/build-compile-commands/build/g compile_commands.json COMMAND_ECHO STDERR)
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

add_subdirectory(vst)
add_subdirectory(structure)
add_subdirectory(interface)
add_subdirectory(interface-gen)
add_subdirectory(interface-persist)
add_subdirectory(global)
add_subdirectory(database)
add_subdirectory(interface-database)
add_subdirectory(script)
add_subdirectory(interface-script)
add_subdirectory(av)

add_library(merc STATIC
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

enable_testing()

add_subdirectory(modules)
add_subdirectory(test)
add_subdirectory(mercury)

add_library(av STATIC
    include/merc/av/change-scope.h
    include/merc/av/editor.h
    include/merc/av/element.h
    include/merc/av/gate.h
    include/merc/av/link.h
    include/merc/av/merc-av.h
    include/merc/av/merc-element.h
    include/merc/av/merc-link.h
    include/merc/av/merc-step.h
    include/merc/av/mercer.h
    include/merc/av/out.h
    include/merc/av/resource.h
    include/merc/av/revolver.h
    include/merc/av/space.h
    include/merc/av/step.h
    include/merc/av/thread.h
    include/merc/av/time-keeper.h
    include/merc/av/vst-element.h
    include/merc/av/vst-link.h
    include/merc/av/vst-step.h
    include/merc/av/work.h
    include/merc/av/worker.h
    source/change-scope.cpp
    source/editor.cpp
    source/element.cpp
    source/merc-av.cpp
    source/merc-element.cpp
    source/mercer.cpp
    source/out.cpp
    source/resource.cpp
    source/space.cpp
    source/time-keeper.cpp
    source/visitors.h
    source/vst-element.cpp
    source/vst-link.cpp
)
target_include_directories(av PUBLIC include)
target_link_libraries(av PUBLIC global merc-vst)
if(WIN32)
    target_sources(av PRIVATE
        source/editor-window.cpp
        source/pixels.cpp
        source/thread.cpp
        source/vst-step.cpp
    )
    target_link_libraries(av INTERFACE d3d12 dxgi d3dcompiler runtimeobject)
    target_compile_options(av PUBLIC "/wd4661")
elseif(APPLE)
    set(OBJC_SOURCES
        source/editor-window.mm
        source/pixels.mm
        source/thread.mm
        source/vst-step.mm
    )
    target_sources(av PRIVATE ${OBJC_SOURCES})
    set_source_files_properties(${OBJC_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
endif()

add_subdirectory(realtime)

add_library(av-realtime STATIC
    include/merc/av/realtime/audio.h
    include/merc/av/realtime/video.h
    include/merc/av/realtime/video-impl.h)
target_include_directories(av-realtime PUBLIC include)
target_link_libraries(av-realtime PUBLIC av)
if(WIN32)
    target_sources(av-realtime PRIVATE
        source/audio.cpp
        source/video.cpp
    )
elseif(APPLE)
    set(OBJC_SOURCES
        source/audio.mm
        source/video.mm
    )
    target_sources(av-realtime PRIVATE ${OBJC_SOURCES})
    set_source_files_properties(${OBJC_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(av-realtime INTERFACE "-framework AudioToolbox")
endif()

add_library(merc-vst STATIC
    include/merc/vst/factory.h
    include/merc/vst/faust.h
    include/merc/vst/graphics.h
    include/merc/vst/ivstvideoprocessor.h
    include/merc/vst/module-instance.h
    include/merc/vst/module.h
    include/merc/vst/video-bus-arrangement.h
    include/merc/vst/voice-pool.h
    include/merc/vst/vst-utils.h
    include/merc/vst/vstvideoeffect.h
    source/faust.cpp
    source/vstvideoeffect.cpp
    ${CMAKE_SOURCE_DIR}/vst3sdk/public.sdk/source/vst/moduleinfo/moduleinfoparser.cpp
    ${CMAKE_SOURCE_DIR}/vst3sdk/public.sdk/source/vst/moduleinfo/moduleinfocreator.cpp)
target_include_directories(merc-vst PUBLIC include)
target_link_libraries(merc-vst PUBLIC sdk sdk_hosting)
if(WIN32)
    target_sources(merc-vst PRIVATE
        ${CMAKE_SOURCE_DIR}/vst3sdk/public.sdk/source/vst/hosting/module_win32.cpp
        include/merc/vst/windows-utils.h
        source/graphics.cpp
        source/windows-utils.cpp)
elseif(APPLE)
    set(OBJC_SOURCES
        ${CMAKE_SOURCE_DIR}/vst3sdk/public.sdk/source/vst/hosting/module_mac.mm
        source/graphics.mm)
    target_sources(merc-vst PRIVATE ${OBJC_SOURCES})
    set_source_files_properties(${OBJC_SOURCES}
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(merc-vst INTERFACE "-framework Metal" "-framework MetalKit" "-framework MetalPerformanceShaders")
endif()
