if(WIN32)
    set(TRIANGLE_SOURCES triangle.cpp)
    set(TRIANGLE_SHADERS triangle.hlsl)
elseif(APPLE)
    set(TRIANGLE_SOURCES triangle.mm metal-types.h)
    set(TRIANGLE_SHADERS triangle.metal)
    set_source_files_properties(triangle.mm
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
endif()

merc_add_module(triangle ${TRIANGLE_SOURCES}
    GUID ed53a5c2-ebc0-43e3-bf6e-05c7db225813
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com
    SHADERS ${TRIANGLE_SHADERS})
add_dependencies(triangle moduleinfotool)
