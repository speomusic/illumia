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
