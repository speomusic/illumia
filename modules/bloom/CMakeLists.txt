if(WIN32)
    set(bloom_SOURCES bloom.cpp)
elseif(APPLE)
    set(bloom_SOURCES bloom.mm metal-types.h)
    set(bloom_SHADERS bloom.metal)
    set_source_files_properties(bloom.mm
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
endif()

merc_add_module(bloom ${bloom_SOURCES}
    SHADERS ${bloom_SHADERS}
    GUID 75227f58-16d8-4ed2-8870-c576f49edde1
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
add_dependencies(bloom moduleinfotool)
