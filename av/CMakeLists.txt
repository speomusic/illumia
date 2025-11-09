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
