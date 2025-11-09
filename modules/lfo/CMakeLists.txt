merc_add_faust(lfo-faust ${CMAKE_CURRENT_SOURCE_DIR}/lfo.dsp)
merc_add_module(lfo lfo.cpp
    GUID 5c24ae97-6e87-44ea-9406-34290d5d1acc
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(lfo PRIVATE lfo-faust)
add_dependencies(lfo moduleinfotool)
