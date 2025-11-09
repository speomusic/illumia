if(WIN32)
    set(texcube_SOURCES texcube.cpp)
    set(texcube_SHADERS texcube.hlsl)
elseif(APPLE)
    set(texcube_SOURCES texcube.mm metal-types.h)
    set(texcube_SHADERS texcube.metal)
    set_source_files_properties(texcube.mm
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
endif()

merc_add_module(texcube ${texcube_SOURCES}
    GUID 1861040b-de00-44b0-8c39-6f3e0ae8f7a4
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com
    SHADERS ${texcube_SHADERS})
add_dependencies(texcube moduleinfotool)
