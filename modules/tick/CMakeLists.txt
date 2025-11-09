merc_add_faust(tick-faust ${CMAKE_CURRENT_SOURCE_DIR}/tick.dsp)
merc_add_module(tick tick.cpp
    GUID 779a9fd9-8c55-4ba9-ab04-1c2d304e5ef3
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(tick PRIVATE tick-faust)
add_dependencies(tick moduleinfotool)
